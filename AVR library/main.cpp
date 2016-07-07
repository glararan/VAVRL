/*
	VETESKA AVR LIBRARY => VAVRL
*/

#include "Settings.h"

#include <avr/io.h>

#include "Serial.h"
#include "DHT.h"
#include "IRRemote.h"
#include "I2C.h"
#include "LCD.h"
#include "TTP229.h"
#include "MatrixKeyboard.h"
#include "LEDStrips.h"
#include "MSGEQ.h"

using namespace VAVRL;

volatile unsigned long VAVRL::Millis = 0;
volatile unsigned long VAVRL::OverflowCount = 0;

uint8_t VAVRL::AnalogReference = 1;

void initAnalog()
{
	sbi(ADCSRA, ADPS2);
	sbi(ADCSRA, ADPS1);
	sbi(ADCSRA, ADPS0);
	sbi(ADCSRA, ADEN);
}

void initTimer()
{
	sbi(TCCR0A, WGM01);
	sbi(TCCR0A, WGM00);
	sbi(TCCR0B, CS01);
	sbi(TCCR0B, CS00);
	sbi(TIMSK0, TOIE0);
}

void initMSGEQ()
{
	MSGEQ7_DDR |= _BV(MSGEQ7_STROBE_PIN); // strobe - output
	MSGEQ7_DDR |= _BV(MSGEQ7_RESET_PIN); // reset - output
	
	MSGEQ7_PORT |= _BV(MSGEQ7_STROBE_PIN);  // strobe high
	MSGEQ7_PORT &= ~_BV(MSGEQ7_RESET_PIN); // reset low
}

void DHT_example()
{
	DHT* dht = new DHT(DHT::DHT11);
	
	while(1)
	{
		if(dht->Process())
			Serial::WriteString("Temp: %d*C. Humidity: %d\n", dht->Temperature(), dht->Humidity());
	}
	
	delete dht;
}

void DHT_example2() // not tested as local var but should work
{
	DHT dht = DHT(DHT::DHT11);
	
	while(1)
	{
		if(dht.Process())
			Serial::WriteString("Temp: %d*C. Humidity: %d\n", dht.Temperature(), dht.Humidity());
	}
}

void I2C_example()
{
	TwoWire.startTransmittion(0x68);
	TwoWire.write(0);
	TwoWire.closeTransmittion();
}

void IR_example()
{
	sei();
	
	IRRemoteDecoder decoder;
	
	IRRemote* remote = new IRRemote();
	
	while(1)
	{
		if(remote->GetCommand(&decoder))
		{
			Serial::WriteString("IR: actual %x, IR: last %x\n", decoder.getValue(), decoder.getLastValue());
			
			remote->Resume();
		}
	}
	
	delete remote;
}

void IR_example2() // not tested as local var but should work
{
	sei();
	
	IRRemoteDecoder decoder;
	
	IRRemote remote;
	
	while(1)
	{
		if(remote.GetCommand(&decoder))
		{
			Serial::WriteString("IR: actual %x, IR: last %x\n", decoder.getValue(), decoder.getLastValue());
			
			remote.Resume();
		}
	}
}

void LCD_example()
{
	LCD* lcd = new LCD(0x27, 16, 2);
	
	lcd->Clear(); // clear display
	lcd->Toggle(true); // toggle on/off display
	lcd->ScrollDisplayLeft(); // scroll to left
	lcd->ScrollDisplayRight(); // scroll to right
	lcd->Home();  // set home
	
	lcd->setBacklight(true); // enable/disable backlight
	lcd->setCursor(true, true); // enable/disable cursor, enable/disable blink (cursor must be enabled)
	lcd->setCursorPosition(0, 0); // set cursor position
	
	lcd->Show("Hello World!"); // print text
	
	delete lcd;
}

void LCD_example2() // not tested as local var but should work
{
	LCD lcd = LCD(0x27, 16, 2);
	
	lcd.Clear(); // clear display
	lcd.Toggle(true); // toggle on/off display
	lcd.ScrollDisplayLeft(); // scroll to left
	lcd.ScrollDisplayRight(); // scroll to right
	lcd.Home();  // set home
	
	lcd.setBacklight(true); // enable/disable backlight
	lcd.setCursor(true, true); // enable/disable cursor, enable/disable blink (cursor must be enabled)
	lcd.setCursorPosition(0, 0); // set cursor position
	
	lcd.Show("Hello World!"); // print text
}

void Serial_example()
{
	Serial::WriteByte(0);
	Serial::WriteChar('a');
	Serial::WriteInt(1024);
	Serial::WriteString("%s", "text");
	Serial::WriteUInt16(1024);
}

void TTP229_example()
{
	TTP229* ttp = new TTP229(16, &DDRB, &PORTB, PORTB0, &DDRB, &PINB, &PORTB, PORTB1);
	
	while(1)
	{
		ButtonEvent button = ttp->Event();
		
		if(button.Value != 0)
			Serial::WriteString("Button %d. %s", button.Value, button.State == None ? "none" : button.State == Pressed ? "pressed" : "released");
	}
	
	delete ttp;
}

void TTP229_example2() // not tested as local var but should work
{
	TTP229 ttp = TTP229(16, &DDRB, &PORTB, PORTB0, &DDRB, &PINB, &PORTB, PORTB1);
	
	while(1)
	{
		ButtonEvent button = ttp.Event();
		
		if(button.Value != 0)
			Serial::WriteString("Button %d. %s", button.Value, button.State == None ? "none" : button.State == Pressed ? "pressed" : "released");
	}
}

void MatrixKeyboard_example() // not tested!!!
{
	MatrixKeyboard keyboard = MatrixKeyboard(&DDRB, &PORTB, &PINB, PORTB0,
											 &DDRB, &PORTB, &PINB, PORTB1,
											 &DDRB, &PORTB, &PINB, PORTB2,
											 &DDRB, &PORTB, &PINB, PORTB3,
											 &DDRC, &PORTC, &PINC, PORTC0,
											 &DDRC, &PORTC, &PINC, PORTC1,
											 &DDRC, &PORTC, &PINC, PORTC2,
											 &DDRC, &PORTC, &PINC, PORTC3);
											 
	while(1)
	{
		uint8_t key = keyboard.GetKey();
	}
}

void LEDstrips_example()
{
	DDRD |= _BV(UCS1903_PIN);

	uint8_t length = 10;
	
	RGB* array = new RGB[length];
	
	srand(0);
	
	while(1)
	{
		for(uint8_t i = 0; i < length; ++i)
			array[i] = KnownColors[random(0, arrayLength(KnownColors))];
		
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			UCS1903::Render(array, length);
		}
	}
}

void MSGEQ_example()
{
	#define MSGEQ7_INTERVAL ReadsPerSecond(35)

	initAnalog();
	initMSGEQ();

	while(1)
	{
		bool newReading = MSGEQ_read(MSGEQ7_INTERVAL);

		if(newReading)
		{
			uint8_t red   = mapNoise(MSGEQ_get(MSGEQ7_BASS), 17);
			uint8_t green = mapNoise(MSGEQ_get(MSGEQ7_5), 46);
			uint8_t blue  = mapNoise(MSGEQ_get(MSGEQ7_6), 27);

			uint16_t volume = MSGEQ_getVolume();

			RGB rgb = RGB(red, green, blue);
		}
	}
}

/// ISR
ISR(TIMER0_OVF_vect)
{
	Millis += MILLIS_INC;
	OverflowCount++;
}