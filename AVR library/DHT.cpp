#include "DHT.h"

#include "Utility.h"
#include "Serial.h"

#include <string.h>

#define _DDR DDRB
#define _PORT PORTB
#define _PINB PINB
#define _PIN  PB2

namespace VAVRL
{
	DHT::DHT(Type DHTtype, volatile uint8_t* DDR, volatile uint8_t* Port, volatile uint8_t* PortPin, uint8_t Pin)
	: type(DHTtype)
	, ddr(DDR)
	, port(Port)
	, portPin(PortPin)
	, pin(Pin)
	, lastReadTime(0)
	{
		/*pinMode(OUTPUT, ddr, port, pin);
		
		digitalWrite(port, pin, HIGH);*/
		
		_DDR  |= _BV(_PIN); // out
		_PORT |= _BV(_PIN); // high
	}
	
	bool DHT::Process()
	{
		sleep(1000);
		
		return Read();
	}
	
	bool DHT::Read()
	{
		uint8_t counter = 0;
		uint8_t j       = 0;
		
		bool lastState = true;
		
		memset(data, 0, 5); // clear
		
		/*digitalWrite(port, pin, HIGH);
		
		sleep(250);*/
		
		// reset
		/*pinMode(OUTPUT, ddr, port, pin);
		
		digitalWrite(port, pin, HIGH);*/
		
		_DDR  |= _BV(_PIN); // out
		_PORT |= _BV(_PIN); // high
		
		sleep(100);
		
		// send request
		//digitalWrite(port, pin, LOW);
		
		_PORT &= ~_BV(_PIN); // low
		
		sleep(18);
		
		//digitalWrite(port, pin, HIGH);
		
		_PORT |= _BV(_PIN); // high
		
		sleep_us(1);
		
		//pinMode(INPUT, ddr, port, pin);
		
		_DDR &= ~_BV(_PIN); // in
		
		sleep_us(50);
		
		// check condition 1
		//if(digitalRead(portPin, pin))
		if((_PINB & _BV(_PIN)))
		{
			Serial::WriteString("End 1");
			
			return false;
		}
			
		sleep_us(80);
			
		// check condition 2
		//if(!digitalRead(portPin, pin))
		if(!(_PINB & _BV(_PIN)))
		{
			Serial::WriteString("End 2");
			
			return false;
		}
			
		sleep_us(80);
		
		for(uint8_t i = 0; i < MAX_TIMING; ++i)
		{
			counter = 0;
			
			while((_PINB & _BV(_PIN)) == lastState)
			{
				counter++;
				
				sleep_us(2);
				
				if(counter == 255)
					break;
			}
			
			lastState = (_PINB & _BV(_PIN));
			
			if(counter == 255)
			{
				char* str = new char[35];
				
				sprintf(str, "breaking on %d\n", i);
				
				Serial::WriteString(str);
				
				delete str;
				
				break;
			}
				
			if((i >= 4) && (i % 2 == 0))
			{
				data[j / 8] <<= 1;
				
				if(counter > 6)
					data[j / 8] |= 1;
					
				j++;
			}
		}
		
		/*char* str = new char[35];
		
		sprintf(str, "%d - %d - %d - %d\n", data[0], data[1], data[2], data[3]);
		
		Serial::WriteString(str);
		
		delete str;*/
		
		if((j >= 40) && (data[4] == (data[0] + data[1] + data[2] + data[3]) & 0xFF))
			return true;
		
		return false;
	}
	
	uint8_t DHT::Temperature()
	{
		int t;
		
		switch(type)
		{
			case DHT11:
			default:
				return data[2];
			
			case DHT21:
			case DHT22:
				{
					t  = data[2] & 0x7F;
					t *= 256;
					t += data[3];
					t /= 10;
					
					if(data[2] & 0x80)
						t *= -1;
					
					return t;
				}
		}
	}
	
	uint8_t DHT::Humidity()
	{
		int h;
		
		switch(type)
		{
			case DHT11:
			default:
				return data[0];
				
			case DHT21:
			case DHT22:
				{
					h  = data[0];
					h *= 256;
					h += data[1];
					h /= 10;
					
					return h;
				}
		}
	}
}