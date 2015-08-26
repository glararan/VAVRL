#include "LEDStrips.h"
#include "Utility.h"

using namespace VAVRL;

namespace VAVRL
{
	void UCS1903::Initialize(volatile uint8_t* port, uint8_t pin)
	{
		Port = port;
		Pin  = pin;
	}
	
	void UCS1903::Render(RGB* array)
	{
		for(uint8_t i = 0; i < (sizeof(array) / sizeof(*array)); ++i)
		{
			for(uint8_t g = 0; g < 8; ++g)
			{
				if(array[i].green & (1 << g))
				{
					digitalWrite(Port, Pin, HIGH);
					
					_delay_us(2);
					
					digitalWrite(Port, Pin, LOW);
					
					_delay_us(0.5);
				}
				else
				{
					digitalWrite(Port, Pin, HIGH);
					
					_delay_us(0.5);
					
					digitalWrite(Port, Pin, LOW);
					
					_delay_us(2);
				}
			}
			
			for(uint8_t r = 0; r < 8; ++r)
			{
				if(array[i].red & (1 << r))
				{
					digitalWrite(Port, Pin, HIGH);
					
					_delay_us(2);
					
					digitalWrite(Port, Pin, LOW);
					
					_delay_us(0.5);
				}
				else
				{
					digitalWrite(Port, Pin, HIGH);
					
					_delay_us(0.5);
					
					digitalWrite(Port, Pin, LOW);
					
					_delay_us(2);
				}
			}
			
			for(uint8_t b = 0; b < 8; ++b)
			{
				if(array[i].blue & (1 << b))
				{
					digitalWrite(Port, Pin, HIGH);
					
					_delay_us(2);
					
					digitalWrite(Port, Pin, LOW);
					
					_delay_us(0.5);
				}
				else
				{
					digitalWrite(Port, Pin, HIGH);
					
					_delay_us(0.5);
					
					digitalWrite(Port, Pin, LOW);
					
					_delay_us(2);
				}
			}
		}
	}
}