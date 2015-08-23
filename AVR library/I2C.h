#include "Settings.h"

#include <stdint.h>
#include <stdlib.h>

#define TWI_FREQ          100000L
#define TWI_BUFFER_LENGTH 32

namespace VAVRL
{
	class TWI
	{
	public:
		enum Status
		{
			Ready,
			MRX,
			MTX,
			SRX,
			STX	
		};
	
		TWI();
		
		void setAddress(uint8_t addr);
		
		uint8_t readFrom(uint8_t address, uint8_t* data, uint8_t length, uint8_t SendStop);
		uint8_t writeTo(uint8_t address, uint8_t* data, uint8_t length, uint8_t wait, uint8_t SendStop);
		uint8_t transmit(const uint8_t* data, uint8_t length);
		
		void attachSlaveRxEvent(void (*function)(uint8_t*, int));
		void attachSlaveTxEvent(void (*function)(void));
		
		void reply(uint8_t ack);
		void stop();
		void releaseBus();
		
		static TWI* Instance;
		
		volatile Status  state;
		volatile uint8_t slarw;
		volatile uint8_t sendStop;   // should the transaction end with a stop
		volatile uint8_t inRepStart; // in the middle of a repeated start

		void (*onSlaveTransmit)(void);
		void (*onSlaveReceive)(uint8_t*, int);

		uint8_t          masterBuffer[TWI_BUFFER_LENGTH];
		volatile uint8_t masterBufferIndex;
		volatile uint8_t masterBufferLength;

		uint8_t          txBuffer[TWI_BUFFER_LENGTH];
		volatile uint8_t txBufferIndex;
		volatile uint8_t txBufferLength;

		uint8_t          rxBuffer[TWI_BUFFER_LENGTH];
		volatile uint8_t rxBufferIndex;

		volatile uint8_t error;
	};
	
	class I2C
	{
	public:
		I2C();
		I2C(uint8_t address);
		
		void   startTransmittion(uint8_t addr);
		int8_t closeTransmittion(uint8_t sendStop = true);
		
		int available() { return rxBufferLength - rxBufferIndex; }
		
		void setClock(uint32_t clock);
		
		size_t write(uint8_t data);
		
		int read();
		
		void onReceive(void (*)(int));
		void onRequest(void (*)(void));
		
	private:
		static uint8_t rxBuffer[];
		static uint8_t rxBufferIndex;
		static uint8_t rxBufferLength;

		static uint8_t txAddress;
		static uint8_t txBuffer[];
		static uint8_t txBufferIndex;
		static uint8_t txBufferLength;

		static uint8_t transmitting;
		static void (*user_onRequest)(void);
		static void (*user_onReceive)(int);
		static void onRequestService(void);
		static void onReceiveService(uint8_t*, int);
		
		void init();
	};
	
	extern I2C TwoWire;
}