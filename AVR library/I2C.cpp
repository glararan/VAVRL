#include "I2C.h"

#include "Utility.h"

#include <compat/twi.h>

#ifndef cbi
	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

namespace VAVRL
{
	TWI* TWI::Instance = new TWI();
	
	TWI::TWI()
	{
		state      = Ready;
		sendStop   = true;
		inRepStart = false;
		
		PORTC |= _BV(PORTC4); // digitalWrite SDA high
		PORTC |= _BV(PORTC5); // digitalWrite SCL high
		
		cbi(TWSR, TWPS0);
		cbi(TWSR, TWPS1);
		
		TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
		TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
	}
	
	void TWI::setAddress(uint8_t addr)
	{
		TWAR = addr << 1;
	}
	
	uint8_t TWI::readFrom(uint8_t address, uint8_t* data, uint8_t length, uint8_t SendStop)
	{
		if(length > TWI_BUFFER_LENGTH)
			return 0;
			
		while(state != Ready)
			continue;
		
		state    = MRX;
		sendStop = SendStop;
		error    = 0xFF;
		
		masterBufferIndex  = 0;
		masterBufferLength = length - 1;
		
		slarw  = TW_READ;
		slarw |= address << 1;
		
		if(inRepStart)
		{
			inRepStart = false;
			
			TWDR = slarw;
			TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
		}
		else
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA);
			
		while(state == MRX)
			continue;
			
		if(masterBufferIndex < length)
			length = masterBufferIndex;
		
		for(uint8_t i = 0; i < length; ++i)
			data[i] = masterBuffer[i];
			
		return length;
	}
	
	uint8_t TWI::writeTo(uint8_t address, uint8_t* data, uint8_t length, uint8_t wait, uint8_t SendStop)
	{
		if(length > TWI_BUFFER_LENGTH)
			return 1;
			
		while(state != Ready)
			continue;
			
		state    = MRX;
		sendStop = SendStop;
		error    = 0xFF;
		
		masterBufferIndex  = 0;
		masterBufferLength = length;
		
		for(uint8_t i = 0; i < length; ++i)
			masterBuffer[i] = data[i];
			
		slarw  = TW_WRITE;
		slarw |= address << 1;
		
		if(inRepStart)
		{
			inRepStart = false;
			
			TWDR = slarw;
			TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
		}
		else
			TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA);
			
		while(wait && state == MRX)
			continue;
			
		if(error == 0xFF)
			return 0; // succ
		else if(error == TW_MT_SLA_NACK)
			return 2; // error: addr send, nack rec
		else if(error == TW_MT_DATA_NACK)
			return 3; // error: data send, nack rec
		else
			return 4; // error: other
	}
	
	uint8_t TWI::transmit(const uint8_t* data, uint8_t length)
	{
		if(length > TWI_BUFFER_LENGTH)
			return 1;
			
		if(state != STX)
			return 2;
			
		txBufferLength = length;
		
		for(uint8_t i = 0; i < length; ++i)
			txBuffer[i] = data[i];
			
		return 0;
	}
	
	void TWI::attachSlaveRxEvent(void (*function)(uint8_t*, int))
	{
		onSlaveReceive = function;
	}
	
	void TWI::attachSlaveTxEvent(void (*function)(void))
	{
		onSlaveTransmit = function;
	}
	
	void TWI::reply(uint8_t ack)
	{
		if(ack)
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
		else
			TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
	}
	
	void TWI::stop()
	{
		TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);
		
		while(TWCR & _BV(TWSTO))
			continue;
			
		state = Ready;
	}
	
	void TWI::releaseBus()
	{
		TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
		
		state = Ready;
	}
	
	ISR(TWI_vect)
	{
		if(!TWI::Instance)
			return;
		
		switch(TW_STATUS)
		{
			case TW_START:
			case TW_REP_START:
				{
					TWDR = TWI::Instance->slarw;
					
					TWI::Instance->reply(1);
				}
				break;
			
			// Master transmitter
			case TW_MT_SLA_ACK:
			case TW_MT_DATA_ACK:
				{
					if(TWI::Instance->masterBufferIndex < TWI::Instance->masterBufferLength)
					{
						TWDR = TWI::Instance->masterBuffer[TWI::Instance->masterBufferIndex++];
						
						TWI::Instance->reply(1);
					}
					else
					{
						if(TWI::Instance->sendStop)
							TWI::Instance->stop();
						else
						{
							TWI::Instance->inRepStart = true;
							
							TWCR = _BV(TWINT) | _BV(TWSTA)| _BV(TWEN);
							
							TWI::Instance->state = TWI::Ready;
						}
					}
				}
				break;
				
			case TW_MT_SLA_NACK:
				{
					TWI::Instance->error = TW_MT_SLA_NACK;
					
					TWI::Instance->stop();
				}
				break;
				
			case TW_MT_DATA_NACK:
				{
					TWI::Instance->error = TW_MT_DATA_NACK;
					
					TWI::Instance->stop();
				}
				break;
				
			case TW_MT_ARB_LOST:
				{
					TWI::Instance->error = TW_MT_ARB_LOST;
					
					TWI::Instance->releaseBus();
				}
				break;
				
			// Master Receive
			case TW_MR_DATA_ACK:
				TWI::Instance->masterBuffer[TWI::Instance->masterBufferIndex++] = TWDR;
				
			case TW_MR_SLA_ACK:
				{
					if(TWI::Instance->masterBufferIndex < TWI::Instance->masterBufferLength)
						TWI::Instance->reply(1);
					else
						TWI::Instance->reply(0);
				}
				break;
				
			case TW_MR_DATA_NACK:
				{
					TWI::Instance->masterBuffer[TWI::Instance->masterBufferIndex++] = TWDR;
					
					if(TWI::Instance->sendStop)
						TWI::Instance->stop();
					else
					{
						TWI::Instance->inRepStart = true;
						
						TWCR = _BV(TWINT) | _BV(TWSTA)| _BV(TWEN);
						
						TWI::Instance->state = TWI::Ready;
					}
				}
				break;
				
			case TW_MR_SLA_NACK:
				TWI::Instance->stop();
				break;
			
			// Slave receiver
			case TW_SR_SLA_ACK:
			case TW_SR_GCALL_ACK:
			case TW_SR_ARB_LOST_SLA_ACK:
			case TW_SR_ARB_LOST_GCALL_ACK:
				{
					TWI::Instance->state         = TWI::SRX;
					TWI::Instance->rxBufferIndex = 0;
					
					TWI::Instance->reply(1);
				}
				break;
				
			case TW_SR_DATA_ACK:
			case TW_SR_GCALL_DATA_ACK:
				{
					if(TWI::Instance->rxBufferIndex < TWI_BUFFER_LENGTH)
					{
						TWI::Instance->rxBuffer[TWI::Instance->rxBufferIndex++] = TWDR;
						
						TWI::Instance->reply(1);
					}
					else
						TWI::Instance->reply(0);
				}
				break;
				
			case TW_SR_STOP:
				{
					if(TWI::Instance->rxBufferIndex < TWI_BUFFER_LENGTH)
						TWI::Instance->rxBuffer[TWI::Instance->rxBufferIndex] = '\0';
						
					TWI::Instance->stop();
					TWI::Instance->onSlaveReceive(TWI::Instance->rxBuffer, TWI::Instance->rxBufferIndex);
					
					TWI::Instance->rxBufferIndex = 0;
					
					TWI::Instance->releaseBus();
				}
				break;
				
			case TW_SR_DATA_NACK:
			case TW_SR_GCALL_DATA_NACK:
				TWI::Instance->reply(0);
				break;
			
			// Slave transmitter
			case TW_ST_SLA_ACK:
			case TW_ST_ARB_LOST_SLA_ACK:
				{
					TWI::Instance->state          = TWI::STX;
					TWI::Instance->txBufferIndex  = 0;
					TWI::Instance->txBufferLength = 0;
					
					TWI::Instance->onSlaveTransmit();
					
					if(TWI::Instance->txBufferLength == 0)
					{
						TWI::Instance->txBufferLength = 1;
						TWI::Instance->txBuffer[0]    = 0x00;
					}
				}
				
			case TW_ST_DATA_ACK:
				{
					TWDR = TWI::Instance->txBuffer[TWI::Instance->txBufferIndex++];
					
					if(TWI::Instance->txBufferIndex < TWI::Instance->txBufferLength)
						TWI::Instance->reply(1);
					else
						TWI::Instance->reply(0);
				}
				break;
				
			case TW_ST_DATA_NACK:
			case TW_ST_LAST_DATA:
				{
					TWI::Instance->reply(1);
					
					TWI::Instance->state = TWI::Ready;
				}
				break;
				
			case TW_NO_INFO:
				break;
				
			case TW_BUS_ERROR:
				{
					TWI::Instance->error = TW_BUS_ERROR;
					
					TWI::Instance->stop();
				}
				break;
		}
	}
	
	/// I2C
	uint8_t I2C::rxBuffer[TWI_BUFFER_LENGTH];
	uint8_t I2C::rxBufferIndex  = 0;
	uint8_t I2C::rxBufferLength = 0;

	uint8_t I2C::txAddress = 0;
	uint8_t I2C::txBuffer[TWI_BUFFER_LENGTH];
	uint8_t I2C::txBufferIndex  = 0;
	uint8_t I2C::txBufferLength = 0;

	uint8_t I2C::transmitting = 0;

	void (*I2C::user_onRequest)(void);
	void (*I2C::user_onReceive)(int);
	
	I2C::I2C()
	{
		init();
	}
	
	I2C::I2C(uint8_t address)
	{
		TWI::Instance->setAddress(address);
		TWI::Instance->attachSlaveTxEvent(onRequestService);
		TWI::Instance->attachSlaveRxEvent(onReceiveService);
		
		init();
	}
	
	void I2C::init()
	{
		rxBufferIndex = rxBufferLength = txBufferIndex = txBufferLength = 0;
	}
	
	void I2C::startTransmittion(uint8_t addr)
	{
		transmitting = 1;
		txAddress    = addr;
		
		txBufferIndex = txBufferLength = 0;
	}
	
	int8_t I2C::closeTransmittion(uint8_t sendStop /* = true */)
	{
		int8_t callback = TWI::Instance->writeTo(txAddress, txBuffer, txBufferLength, 1, sendStop);
		
		txBufferIndex = txBufferLength = transmitting = 0;
		
		return callback;
	}
	
	void I2C::setClock(uint32_t clock)
	{
		TWBR = ((F_CPU / clock) - 16) / 2;
	}
	
	size_t I2C::write(uint8_t data)
	{
		if(transmitting)
		{
			if(txBufferLength >= TWI_BUFFER_LENGTH)
				return 0;
			
			txBuffer[txBufferIndex++] = data;
			
			txBufferLength = txBufferIndex;
		}
		else
			TWI::Instance->transmit(&data, 1);
		
		return 1;
	}
	
	int I2C::read()
	{
		int value = -1;
		
		if(rxBufferIndex < rxBufferLength)
			value = rxBuffer[rxBufferIndex++];
			
		return value;
	}
	
	uint8_t I2C::requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop /* = true */)
	{
		if(isize > 0)
		{
			startTransmittion(address);
			
			if(isize > 3)
				isize = 3;
			
			while(isize-- > 0)
				write((uint8_t)(iaddress >> (isize * 8)));
			
			closeTransmittion(false);
		}
		
		if(quantity > TWI_BUFFER_LENGTH)
			quantity = TWI_BUFFER_LENGTH;
			
		uint8_t result = TWI::Instance->readFrom(address, rxBuffer, quantity, sendStop);
		
		rxBufferIndex  = 0;
		rxBufferLength = result;
		
		return result;
	}
	
	uint8_t I2C::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop /* = true */)
	{
		return requestFrom(address, quantity, 0, 0, sendStop);
	}
	
	void I2C::onReceive(void (*function)(int))
	{
		user_onReceive = function;
	}
	
	void I2C::onRequest(void (*function)(void))
	{
		user_onRequest = function;
	}
	
	void I2C::onReceiveService(uint8_t* inBytes, int numBytes)
	{
		if(!user_onReceive)
			return;
			
		if(rxBufferLength > rxBufferIndex)
			return;
			
		for(uint8_t i = 0; i < numBytes; ++i)
			rxBuffer[i] = inBytes[i];
			
		rxBufferIndex  = 0;
		rxBufferLength = numBytes;
		
		user_onReceive(numBytes);
	}
	
	void I2C::onRequestService()
	{
		if(!user_onRequest)
			return;
			
		txBufferIndex = txBufferLength = 0;
		
		user_onRequest();
	}
	
	I2C TwoWire = I2C();
}