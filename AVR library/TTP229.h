#ifndef TTP229_H
#define TTP229_H

#include "Utility.h"

namespace VAVRL
{
	enum ButtonState
	{
		None     = 0,
		Pressed  = 1,
		Released = 2
	};
	
	struct ButtonEvent
	{
		uint8_t Value;
		
		ButtonState State;
	};
	
	class TTP229
	{
	public:
		TTP229(uint8_t keysCount, volatile uint8_t* sclDDR, volatile uint8_t* sclPort, uint8_t sclPin, volatile uint8_t* sdoDDR, volatile uint8_t* sdoPortPin, volatile uint8_t* sdoPort, uint8_t sdoPin);
		
		ButtonEvent Event();
		
		uint16_t Read();
		
		uint8_t Keys() { return keys; }
		
	private:
		uint16_t key_state;
		uint8_t  keys; // keys count
		
		volatile uint8_t* SCLport;
		uint8_t  SCLpin;
		
		volatile uint8_t* SDOportPin;
		volatile uint8_t* SDOport;
		uint8_t  SDOpin;
		
		uint16_t previousState;
		
		uint16_t ReadForEvent();
		
		uint8_t ButtonFromFlag(uint16_t changes);
	};
}

#endif // TTP229_H