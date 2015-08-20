#include <stdint.h>

namespace VAVRL
{
	struct RGB
	{
		uint8_t red, green, blue;
		
		RGB(uint8_t r, uint8_t g, uint8_t b)
		{
			red   = r;
			green = g;
			blue  = b;
		}
		
		RGB()
		{
		}
		
		inline bool operator==(RGB second)
		{
			if(red == second.red && green == second.green && blue == second.blue)
				return true;
			
			return false;
		}
	};
	
	RGB KnownColors[] =
	{
		RGB(255, 255, 255), // white
		RGB(255,   0,   0), // red
		RGB(  0, 255,   0), // green
		RGB(  0,   0, 255), // blue
		RGB(255, 128, 128), // pink
		RGB(255, 255,   0), // yellow
		RGB(  0, 128, 255), // light blue
		RGB(255, 128,   0), // orange
		RGB(128,   0, 255), // purple
		RGB(128, 255,   0), // limet
		RGB(255,   0, 128)  // pink-purple
	};
}