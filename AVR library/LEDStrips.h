#include "RGB.h" // include stdint.h

namespace VAVRL
{
	class UCS1903
	{
	public:
		static void Render(RGB* array);
		static void Initialize(uint8_t* port, uint8_t pin);
		
	private:
		static uint8_t* Port;
		static uint8_t Pin;
	};
}