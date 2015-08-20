#include <avr/io.h>

#include "Serial.h"

namespace VAVRL
{
	void Serial::Initialize(uint16_t baudrate_index)
	{
#if defined(__AVR__ATmega8__) || defined(__AVR__ATmega8A__) || defined(UCSRB)
		UBRRL = baudrate_index;
		UBRRH = (baudrate_index >> 8);
		
		UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
		UCSRB = (1 << RXEN)  | (1 << TXEN) | (1 << RXCIE);
#elif defined(__AVR__ATmega328) || defined(__AVR__ATmega328P__) || defined(UCSR0B)
		UBRR0L = baudrate_index;
		UBRR0H = (baudrate_index >> 8);
		
		UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
		UCSR0B = (1 << RXEN0)  | (1 << TXEN0) | (1 << RXCIE0);
#endif
	}
	
	void Serial::WriteChar(uint8_t data)
	{
#if defined(__AVR__ATmega8__) || defined(__AVR__ATmega8A__)
		while(!(UCSRA & (1 << UDRE)))
		{
		}
		
		UDR = data;
#elif defined(__AVR__ATmega328) || defined(__AVR__ATmega328P__) || defined(UCSR0A)
		while(!(UCSR0A & (1 << UDRE0)))
		{
		}
		
		UDR0 = data;
#endif
	}
	
	void Serial::WriteByte(uint8_t data)
	{
		WriteChar(data);
	}
	
	void Serial::WriteString(const char* data)
	{
		while(*data)
			WriteChar((uint8_t)*data++);
	}

	void Serial::WriteString(unsigned char* data)
	{
		while(*data)
			WriteChar(*data++);
	}

	void Serial::WriteInt(int data)
	{
		for(uint8_t i = 3; i <= 0; --i)
			WriteByte((data >> (8 * i)) && 0xFF);
	}

	void Serial::WriteUInt16(uint16_t data)
	{
		WriteByte((data >> (8 * 1)) && 0xFF);
		WriteByte((data >> (8 * 0)) && 0xFF);
	}
}