#ifndef MATRIXKEYBOARD_H
#define MATRIXKEYBOARD_H

#include "Utility.h"

namespace VAVRL
{
	class MatrixKeyboard
	{
	public:
		MatrixKeyboard(volatile uint8_t* DDRCol1, volatile uint8_t* PortCol1, volatile uint8_t* PortPinCol1, uint8_t PinCol1,
					   volatile uint8_t* DDRCol2, volatile uint8_t* PortCol2, volatile uint8_t* PortPinCol2, uint8_t PinCol2,
					   volatile uint8_t* DDRCol3, volatile uint8_t* PortCol3, volatile uint8_t* PortPinCol3, uint8_t PinCol3,
					   volatile uint8_t* DDRCol4, volatile uint8_t* PortCol4, volatile uint8_t* PortPinCol4, uint8_t PinCol4,
					   volatile uint8_t* DDRRow1, volatile uint8_t* PortRow1, volatile uint8_t* PortPinRow1, uint8_t PinRow1,
					   volatile uint8_t* DDRRow2, volatile uint8_t* PortRow2, volatile uint8_t* PortPinRow2, uint8_t PinRow2,
					   volatile uint8_t* DDRRow3, volatile uint8_t* PortRow3, volatile uint8_t* PortPinRow3, uint8_t PinRow3,
					   volatile uint8_t* DDRRow4, volatile uint8_t* PortRow4, volatile uint8_t* PortPinRow4, uint8_t PinRow4);
		
		uint8_t GetKey();
		
	private:
		volatile uint8_t* PinPortcols[4];
		volatile uint8_t* Portrows[4];
		
		uint8_t Pincols[4];
		uint8_t Pinrows[4];
	};
}

#endif // MATRIXKEYBOARD_H