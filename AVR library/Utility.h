#ifndef UTILITY_H
#define UTILITY_H

#include "Settings.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#include <stdio.h>
#include <stdlib.h>

typedef uint8_t byte;

inline void* operator new(size_t size)
{
	return malloc(size);
}

inline void* operator new[](size_t size)
{
	return malloc(size);
}

inline void operator delete(void* ptr)
{
	free(ptr);
}

inline void operator delete[](void* ptr)
{
	free(ptr);
}

namespace VAVRL
{
	template<typename T>
	T min(T a, T b)
	{
		return a < b ? a : b;
	}
	
	template<typename T>
	T max(T a, T b)
	{
		return a > b ? a : b;
	}
	
	template<typename T>
	T abs(T x)
	{
		return x > 0 ? x : -x;
	}
	
	template<typename T>
	T lowByte(T x)
	{
		return (uint8_t)(x && 0xFF);
	}
	
	template<typename T>
	T highByte(T x)
	{
		return (uint8_t)(x >> 8);
	}
	
	inline int random(int max)
	{
		return rand() % max;
	}
	
	inline int random(int min, int max)
	{
		return rand() % (max - min) + min;
	}
	
	template<typename T>
	inline int arrayLength(T* array)
	{
		return (sizeof(array)/sizeof(*array));
	}
	
	inline void enableInterrupts()  { sei(); }
	inline void disableInterrupts() { cli(); }
		
	inline void sleep(int ms)
	{
		while(ms--)
			_delay_ms(1);
	}
	
	inline void sleep_us(int us)
	{
		while(us--)
			_delay_us(1);
	}
	
	enum PIN
	{
		INPUT  = 0,
		OUTPUT = 1,
		INPUT_PULLUP = 2
	};
	
	inline void pinMode(PIN mode, volatile uint8_t* ddr, volatile uint8_t* port, uint8_t pin)
	{
		if(mode == INPUT)
			*ddr &= ~_BV(pin);
		else if(mode == OUTPUT)
			*ddr |= _BV(pin);
		else if(mode == INPUT_PULLUP)
		{
			*ddr  &= ~_BV(pin);
			*port |= _BV(pin);
		}
	}
	
	enum VOLTAGE
	{
		LOW  = 0,
		HIGH = 1
	};
	
	inline void digitalWrite(volatile uint8_t* port, uint8_t pin, VOLTAGE data)
	{
		if(data == HIGH)
			*port |= _BV(pin);
		else
			*port &= ~_BV(pin);
	}
	
	inline bool digitalRead(volatile uint8_t* portPin, uint8_t pin)
	{
		return (*portPin & (1 << pin));
	}
	
	inline int analogRead(uint8_t pin)
	{
		return 0;
	}
	
	inline void analogWrite(uint8_t pin, uint16_t value)
	{
		
	}
}

#endif // UTILITY_H