#ifndef LEDSTRIPS_H
#define LEDSTRIPS_H

#include "Color.h" // include stdint.h

namespace VAVRL
{
	class UCS1903
	{
	public:
		static void Render(RGB* array, uint16_t length);
		static void RenderLamp(RGB* array, uint16_t length);

	private:
		static void setBit(uint8_t data, uint8_t n);
		static void setBits(uint8_t data);
	};
}

#endif // LEDSTRIPS_H