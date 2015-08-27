#ifndef LEDSTRIPS_H
#define LEDSTRIPS_H

#include "RGB.h" // include stdint.h

typedef unsigned int uint16_t;

namespace VAVRL
{
	class UCS1903
	{
	public:
		static void Render(RGB* array, uint16_t length);
	};
}

#endif // LEDSTRIPS_H