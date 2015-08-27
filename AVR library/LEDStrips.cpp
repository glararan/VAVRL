#include "LEDStrips.h"
#include "Utility.h"

using namespace VAVRL;

namespace VAVRL
{	
	void UCS1903::Render(RGB* array, uint16_t length)
	{
		for(uint16_t i = 0; i < length; ++i)
		{
			for(uint8_t g = 0; g < 8; ++g)
			{
				if(array[i].green & (1 << g))
				{
					UCS1903_PORT |= _BV(UCS1903_PIN);
					
					_delay_us(2);
					
					UCS1903_PORT &= ~_BV(UCS1903_PIN);
					
					_delay_us(0.5);
				}
				else
				{
					UCS1903_PORT |= _BV(UCS1903_PIN);
					
					_delay_us(0.5);
					
					UCS1903_PORT &= ~_BV(UCS1903_PIN);
					
					_delay_us(2);
				}
			}
			
			for(uint8_t r = 0; r < 8; ++r)
			{
				if(array[i].red & (1 << r))
				{
					UCS1903_PORT |= _BV(UCS1903_PIN);
					
					_delay_us(2);
					
					UCS1903_PORT &= ~_BV(UCS1903_PIN);
					
					_delay_us(0.5);
				}
				else
				{
					UCS1903_PORT |= _BV(UCS1903_PIN);
					
					_delay_us(0.5);
					
					UCS1903_PORT &= ~_BV(UCS1903_PIN);
					
					_delay_us(2);
				}
			}
			
			for(uint8_t b = 0; b < 8; ++b)
			{
				if(array[i].blue & (1 << b))
				{
					UCS1903_PORT |= _BV(UCS1903_PIN);
					
					_delay_us(2);
					
					UCS1903_PORT &= ~_BV(UCS1903_PIN);
					
					_delay_us(0.5);
				}
				else
				{
					UCS1903_PORT |= _BV(UCS1903_PIN);
					
					_delay_us(0.5);
					
					UCS1903_PORT &= ~_BV(UCS1903_PIN);
					
					_delay_us(2);
				}
			}
		}
	}
}