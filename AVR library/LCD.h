#include "Utility.h"

namespace VAVRL
{
	// commands
	#define LCD_CLEARDISPLAY   0x01
	#define LCD_RETURNHOME     0x02
	#define LCD_ENTRYMODESET   0x04
	#define LCD_DISPLAYCONTROL 0x08
	#define LCD_CURSORSHIFT    0x10
	#define LCD_FUNCTIONSET    0x20
	#define LCD_SETCGRAMADDR   0x40
	#define LCD_SETDDRAMADDR   0x80

	// flags for display entry mode
	#define LCD_ENTRYRIGHT          0x00
	#define LCD_ENTRYLEFT           0x02
	#define LCD_ENTRYSHIFTINCREMENT 0x01
	#define LCD_ENTRYSHIFTDECREMENT 0x00

	// flags for display on/off control
	#define LCD_DISPLAYON  0x04
	#define LCD_DISPLAYOFF 0x00
	#define LCD_CURSORON   0x02
	#define LCD_CURSOROFF  0x00
	#define LCD_BLINKON    0x01
	#define LCD_BLINKOFF   0x00

	// flags for display/cursor shift
	#define LCD_DISPLAYMOVE 0x08
	#define LCD_CURSORMOVE  0x00
	#define LCD_MOVERIGHT   0x04
	#define LCD_MOVELEFT    0x00

	// flags for function set
	#define LCD_8BITMODE 0x10
	#define LCD_4BITMODE 0x00
	#define LCD_2LINE    0x08
	#define LCD_1LINE    0x00
	#define LCD_5x10DOTS 0x04
	#define LCD_5x8DOTS  0x00

	// flags for backlight control
	#define LCD_BACKLIGHT   0x08
	#define LCD_NOBACKLIGHT 0x00
	
	#define En 0B00000100  // Enable bit
	#define Rw 0B00000010  // Read/Write bit
	#define Rs 0B00000001  // Register select bit
	
	class LCD
	{
	public:	
		LCD(uint8_t addr, uint8_t cols, uint8_t rows, uint8_t charsize = 0);
		
		void Clear();
		void Home();
		void Toggle(bool on = true);
		void ScrollDisplayLeft();
		void ScrollDisplayRight();
		
		void Show(const char* str);
		
		void setBacklight(bool state);
		void setCursor(bool state, bool blink = false);
		void setCursorPosition(uint8_t col, uint8_t row);
		
		void command(uint8_t cmd);
		
		void write(uint8_t data);
		
	private:
		void init_priv();
		void send(uint8_t, uint8_t);
		void write4bits(uint8_t);
		void expanderWrite(uint8_t);
		void pulseEnable(uint8_t);
		
		uint8_t address;
		uint8_t numLines;
		uint8_t displayFunction;
		uint8_t displayControl;
		uint8_t displayMode;
		uint8_t backlight;
	};
}