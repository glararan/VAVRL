#include "LCD.h"

#include "I2C.h"

#include <string.h>

namespace VAVRL
{
	LCD::LCD(uint8_t addr, uint8_t cols, uint8_t rows, uint8_t charsize /* = 0 */)
	: address(addr)
	, numLines(rows)
	, backlight(LCD_BACKLIGHT)
	{
		displayFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
		
		if(rows > 1)
			displayFunction |= LCD_2LINE;
		
		if(charsize != 0 && rows == 1)
			displayFunction |= LCD_5x10DOTS;
			
		sleep(50);
		
		expanderWrite(backlight);
		
		sleep(1000);
		
		write4bits(0x03 << 4);
		sleep_us(4500);
		
		write4bits(0x03 << 4);
		sleep_us(4500);
		
		write4bits(0x03 << 4);
		sleep_us(150);
		
		write4bits(0x02 << 4);
		
		command(LCD_FUNCTIONSET | displayFunction);
		
		displayControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
		
		Toggle();
		Clear();
		
		displayMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
		
		command(LCD_ENTRYMODESET | displayMode);
		
		Home();
	}
	
	void LCD::Clear()
	{
		command(LCD_CLEARDISPLAY);
		
		sleep_us(2000);
	}
	
	void LCD::Home()
	{
		command(LCD_RETURNHOME);
		
		sleep_us(2000);
	}
	
	void LCD::Toggle(bool on /* = true */)
	{
		if(on)
			displayControl |= LCD_DISPLAYON;
		else
			displayControl &= ~LCD_DISPLAYON;
		
		command(LCD_DISPLAYCONTROL | displayControl);
	}
	
	void LCD::ScrollDisplayLeft()
	{
		command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
	}
	
	void LCD::ScrollDisplayRight()
	{
		command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
	}
	
	void LCD::Show(const char* str)
	{
		size_t length = strlen(str);
		
		const uint8_t* buffer = (const uint8_t*)str;
		
		while(length--)
			write(*buffer++);
	}
	
	void LCD::setBacklight(bool state)
	{
		if(on)
			backlight = LCD_BACKLIGHT;
		else
			backlight = LCD_NOBACKLIGHT;
		
		expanderWrite(0);
	}
	
	void LCD::setCursor(bool state, bool blink /* = false */)
	{
		if(on)
		{
			displayControl |= LCD_CURSORON;
			
			command(LCD_DISPLAYCONTROL | displayControl);
		}
		else
		{
			_displaycontrol &= ~LCD_CURSORON;
			
			command(LCD_DISPLAYCONTROL | displayControl);
		}
		
		if(blink)
		{
			displayControl |= LCD_BLINKON;
			
			command(LCD_DISPLAYCONTROL | displayControl);
		}
		else
		{
			displayControl &= ~LCD_BLINKON;
			
			command(LCD_DISPLAYCONTROL | displayControl);
		}
	}

	void LCD::setCursorPosition(uint8_t col, uint8_t row)
	{
		int offsets[] = {0x00, 0x40, 0x14, 0x54};
		
		if(row > numLines)
			row = numLines - 1;
		
		command(LCD_SETDDRAMADDR | (col + offsets[row]));
	}
	
	void LCD::command(uint8_t cmd)
	{
		send(cmd, 0);
	}
	
	void LCD::write(uint8_t data)
	{
		send(data, Rs);
	}
	
	void LCD::send(uint8_t cmd, uint8_t mode)
	{
		uint8_t high = cmd & 0xF0;
		uint8_t low  = (cmd << 4) & 0xF0;
		
		write4bits(high | mode);
		write4bits(low  | mode);
	}
	
	void LCD::write4bits(uint8_t bits)
	{
		expanderWrite(bits);
		pulseEnable(bits);
	}
	
	void LCD::expanderWrite(uint8_t data)
	{
		TwoWire.startTransmittion(_Addr);
		TwoWire.write(data | _backlightval);
		TwoWire.closeTransmittion();
	}
	
	void LCD::pulseEnable(uint8_t data)
	{
		expanderWrite(data | En);
		sleep_us(1);
		
		expanderWrite(data & ~En);
		sleep_us(50);
	}
}