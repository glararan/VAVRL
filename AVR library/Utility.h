#ifndef UTILITY_H
#define UTILITY_H

#include "Settings.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <util/atomic.h>

#include <stdio.h>
#include <stdlib.h>

#define clockCyclesPerMicrosecond() (F_CPU / 1000000L)
#define clockCyclesToMicroseconds(a) (a / clockCyclesPerMicrosecond())
#define microsecondsToClockCycles(a) (a * clockCyclesPerMicrosecond())

#define MILLIS_INC (((64 * 256) / (F_CPU / 1000000L)) / 1000)

#ifndef cbi
	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

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
	extern volatile unsigned long Millis;
	extern volatile unsigned long OverflowCount;

	extern uint8_t AnalogReference;

	inline unsigned long micros()
	{
		unsigned long m;
		uint8_t t;

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			m = OverflowCount;

			#if defined(TCNT0)
				t = TCNT0;
			#elif defined(TCNT0L)
				t = TCNT0L;
			#else
				#error TIMER 0 not defined
			#endif

			#ifdef TIFR0
				if ((TIFR0 & _BV(TOV0)) && t < 255)
					m++;
			#else
				if ((TIFR & _BV(TOV0)) && t < 255)
					m++;
			#endif
		}

		return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
	}

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

	template<typename T>
	T constrain(T amt, T low, T high)
	{
		return amt < low ? low : (amt > high ? high : amt);
	}

	template<typename T>
	T map(T x, T in_min, T in_max, T out_min, T out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}
	
	inline int random(int max)
	{
		return rand() % max;
	}
	
	inline int random(int min, int max)
	{
		return rand() % (max - min) + min;
	}
	
	template<unsigned N, typename T>
	inline int arrayLength(T (&array)[N])
	{
		return (sizeof(array)/sizeof(array[0]));
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
	
	inline uint16_t analogRead(uint8_t pin)
	{
		uint8_t low, high;
		
		ADMUX = (AnalogReference << 6) | (pin & 0x07);
		
		sbi(ADCSRA, ADSC);
		
		while(bit_is_set(ADCSRA, ADSC));
		
		low  = ADCL;
		high = ADCH;
		
		return (high << 8) | low;
	}
	
	inline void analogWrite(uint8_t pin, uint16_t value)
	{
		
	}
}

#endif // UTILITY_H