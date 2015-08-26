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

using namespace VAVRL;

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
	UCS1903::Initialize(&PINB, PORTB0);
	
	RGB array[10];
	
	srand(0);
	
	while(1)
	{
		for(uint8_t i = 0; i < 10; ++i)
			array[i] = KnownColors[random(0, arrayLength(KnownColors))];
		
		UCS1903::Render(&array);
	}
}