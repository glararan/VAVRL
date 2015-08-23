#include "DHT.h"

#include "Utility.h"
#include "Serial.h"

#ifndef DHT_DDR
	#define DHT_DDR  DDRB
	#define DHT_PORT PORTB
	#define DHT_PINP PINB
	#define DHT_PIN  PORTB1
#endif

namespace VAVRL
{
	DHT::DHT(Type DHTtype)
	: type(DHTtype)
	, lastReadTime(0)
	{
	}
	
	bool DHT::Process()
	{
		sleep(1000);
		
		return Read();
	}
	
	bool DHT::Read()
	{
		uint8_t counter = 0, j = 0, lastState = 0;
		
		data[0] = data[1] = data[2] = data[3] = data[4] = 0;
		
		cli();
			
		// Reset
		DHT_DDR  |= _BV(DHT_PIN); // output
		DHT_PORT |= _BV(DHT_PIN); // high
		
		sleep(250);
			
		// Request signal
		DHT_PORT &= ~_BV(DHT_PIN); // low
		
		sleep(20);
			
		DHT_PORT |= _BV(DHT_PIN); // high
		
		sleep_us(40);
		
		DHT_DDR &= ~_BV(DHT_PIN); // input

		lastState = DHT_PINP & _BV(DHT_PIN);
			
		// Repeat for each Transistions
		for(uint8_t i = 0; i < MAX_TIMING; ++i)
		{
			counter = 0;
			
			while(lastState == (DHT_PINP & _BV(DHT_PIN)))
			{
				sleep_us(1);
				
				++counter;

				if(counter == 255)
					break;
			}
				
			if(counter == 255)
				break;
			
			if(lastState == _BV(DHT_PIN) && i > 2)
			{
				data[j / 8] <<= 1;
				
				if(counter >= 40)
					data[j / 8] |= 1;
				
				++j;
			}
			
			lastState = DHT_PINP & _BV(DHT_PIN);
		}
		
		sei();
			
		// checksum
		if(data[0] + data[1] + data[2] + data[3] == data[4])
			return true;
		else
			Serial::WriteString("%d - %d - %d - %d = %d\n", data[0], data[1], data[2], data[3], data[4]);
			
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