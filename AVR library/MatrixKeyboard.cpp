#include "MatrixKeyboard.h"

namespace VAVRL
{
	uint8_t keypad[] =
	{
		'1', '2', '3', 'A',
		'4', '5', '6', 'B',
		'7', '8', '9', 'C',
		'*', '0', '#', 'D'
	};
	
	MatrixKeyboard::MatrixKeyboard(volatile uint8_t* DDRCol1, volatile uint8_t* PortCol1, volatile uint8_t* PortPinCol1, uint8_t PinCol1,
								   volatile uint8_t* DDRCol2, volatile uint8_t* PortCol2, volatile uint8_t* PortPinCol2, uint8_t PinCol2,
								   volatile uint8_t* DDRCol3, volatile uint8_t* PortCol3, volatile uint8_t* PortPinCol3, uint8_t PinCol3,
								   volatile uint8_t* DDRCol4, volatile uint8_t* PortCol4, volatile uint8_t* PortPinCol4, uint8_t PinCol4,
								   volatile uint8_t* DDRRow1, volatile uint8_t* PortRow1, volatile uint8_t* PortPinRow1, uint8_t PinRow1,
								   volatile uint8_t* DDRRow2, volatile uint8_t* PortRow2, volatile uint8_t* PortPinRow2, uint8_t PinRow2,
								   volatile uint8_t* DDRRow3, volatile uint8_t* PortRow3, volatile uint8_t* PortPinRow3, uint8_t PinRow3,
								   volatile uint8_t* DDRRow4, volatile uint8_t* PortRow4, volatile uint8_t* PortPinRow4, uint8_t PinRow4)
	{
		// cols
		PinPortcols[0] = PortPinCol1;
		PinPortcols[1] = PortPinCol2;
		PinPortcols[2] = PortPinCol3;
		PinPortcols[3] = PortPinCol4;
		
		Pincols[0] = PinCol1;
		Pincols[1] = PinCol2;
		Pincols[2] = PinCol3;
		Pincols[3] = PinCol4;
		
		// rows		
		Portrows[0] = PortRow1;
		Portrows[1] = PortRow2;
		Portrows[2] = PortRow3;
		Portrows[3] = PortRow4;
		
		Pinrows[0] = PinRow1;
		Pinrows[1] = PinRow2;
		Pinrows[2] = PinRow3;
		Pinrows[3] = PinRow4;
		
		/// pin modes
		// rows outputs
		*DDRRow1 |= _BV(PinRow1);
		*DDRRow2 |= _BV(PinRow2);
		*DDRRow3 |= _BV(PinRow3);
		*DDRRow4 |= _BV(PinRow4);
		
		// cols inputs
		*DDRCol1 &= ~_BV(PinCol1);
		*DDRCol2 &= ~_BV(PinCol2);
		*DDRCol3 &= ~_BV(PinCol3);
		*DDRCol4 &= ~_BV(PinCol4);
	}
	
	uint8_t MatrixKeyboard::GetKey()
	{
		uint8_t character = 0;
		
		for(uint8_t i = 0; i < 4; ++i) // rows
		{
			*Portrows[i] |= _BV(Pinrows[i]);
			
			asm volatile("nop\n\t"::);

			if(*PinPortcols[0] & _BV(Pincols[0]))
				character = 4 * i;
			else if(*PinPortcols[1] & _BV(Pincols[1]))
				character = 4 * i + 1;
			else if(*PinPortcols[2] & _BV(Pincols[2]))
				character = 4 * i + 2;
			else if(*PinPortcols[3] & _BV(Pincols[3]))
				character = 4 * i + 3;

			sleep(45);
			
			*Portrows[i] &= ~_BV(Pinrows[i]);
		}
		
		return keypad[character];
	}
}