#include "TTP229.h"

namespace VAVRL
{
	TTP229::TTP229(uint8_t keysCount, volatile uint8_t* sclDDR, volatile uint8_t* sclPort, uint8_t sclPin, volatile uint8_t* sdoDDR, volatile uint8_t* sdoPortPIN, volatile uint8_t* sdoPort, uint8_t sdoPin)
	: keys(keysCount)
	, SCLport(sclPort)
	, SCLpin(sclPin)
	, SDOportPin(sdoPortPIN)
	, SDOport(sdoPort)
	, SDOpin(sdoPin)
	{
		pinMode(OUTPUT, sclDDR, sclPort, sclPin); // SCL
		pinMode(INPUT,  sdoDDR, sdoPort, sdoPin); // SDO
	}
	
	uint16_t TTP229::Read()
	{
		uint16_t buttonState = 0;
		
		for(uint8_t i = 1; i <= keys; ++i)
		{
			digitalWrite(SCLport, SCLpin, LOW);
			
			if(!digitalRead(SDOportPin, SDOpin))
				buttonState = i;
				
			digitalWrite(SCLport, SCLpin, HIGH);
		}
		
		key_state = buttonState;
		
		return buttonState;
	}
	
	uint16_t TTP229::ReadForEvent()
	{
		uint16_t buttonState = 0;
		
		for(uint8_t i = 0; i < keys; ++i) // sleep for some kind of slow data reading
		{
			digitalWrite(SCLport, SCLpin, LOW);
			
			sleep_us(1);
			
			digitalWrite(SCLport, SCLpin, HIGH);
			
			sleep_us(1);
			
			if(!digitalRead(SDOportPin, SDOpin))
				buttonState |= _BV(i);
		}
		
		return buttonState;
	}
	
	uint8_t TTP229::ButtonFromFlag(uint16_t changes)
	{
		uint16_t flag = 1;
		
		for(uint8_t i = 1; i <= keys; ++i, flag <<= 1)
		{
			if((changes & flag) != 0)
			{
				if((changes & ~flag) != 0)
					i |= 0x80; // some other bit is present
					
				return i;
			}
		}
		
		return 0;
	}
	
	ButtonEvent TTP229::Event()
	{
		ButtonEvent event;
		
		uint16_t currState = ReadForEvent();
		
		uint16_t changes  = previousState ^ currState;
		uint16_t pressed  = (changes & currState);
		uint16_t released = (changes & previousState);
		
		if(pressed != 0)
		{
			uint8_t buttonNumber = ButtonFromFlag(pressed);
			
			event.value = (buttonNumber & 0x7F);
			event.state = Pressed;
			
			previousState |= _BV(event.value - 1);
		}
		else if(released != 0)
		{
			uint8_t buttonNumber = ButtonFromFlag(released);
			
			event.value = (buttonNumber & 0x7F);
			event.state = Released;
			
			previousState &= ~_BV(event.value - 1);
		}
		else
		{
			event.value = 0;
			event.state = None;
		}
		
		return event;
	}
}