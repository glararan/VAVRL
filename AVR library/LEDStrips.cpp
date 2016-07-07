#include "LEDStrips.h"
#include "Utility.h"

using namespace VAVRL;

#define LEDPORT
#define LEDPIN

namespace VAVRL
{
	void UCS1903::Render(RGB* array, uint16_t length)
	{
		#undef LEDPORT
		#undef LEDPIN
		#define LEDPORT UCS1903_PORT
		#define LEDPIN  UCS1903_PIN

		for(uint16_t i = 0; i < length; ++i)
		{
			setBits(array[i].green);
			setBits(array[i].red);
			setBits(array[i].blue);
		}
	}
	
	void UCS1903::RenderLamp(RGB* array, uint16_t length)
	{
		#undef LEDPORT
		#undef LEDPIN
		#define LEDPORT UCS1903_LAMPPORT
		#define LEDPIN  UCS1903_LAMPPIN

		for(uint16_t i = 0; i < length; ++i)
		{
			setBits(array[i].green);
			setBits(array[i].red);
			setBits(array[i].blue);
		}
	}

	void UCS1903::setBit(uint8_t data, uint8_t n)
	{
		if(data & _BV(n))
		{
			LEDPORT |= _BV(LEDPIN);

			_delay_us(2);

			LEDPORT &= ~_BV(LEDPIN);

			_delay_us(0.5);
		}
		else
		{
			LEDPORT |= _BV(LEDPIN);

			_delay_us(0.5);

			LEDPORT &= ~_BV(LEDPIN);

			_delay_us(2);
		}
	}

	void UCS1903::setBits(uint8_t data)
	{
		setBit(data, 7);
		setBit(data, 6);
		setBit(data, 5);
		setBit(data, 4);
		setBit(data, 3);
		setBit(data, 2);
		setBit(data, 1);
		setBit(data, 0);
	}
}