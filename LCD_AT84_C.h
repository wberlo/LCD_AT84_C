/*
 * LCD_AT84_C.h
 *
 * Created: 2015-05-16 18:59:06
 *  Author: Wim
 * Note:
 * This is basically the LiquidCrystal library for arduino, found at
 * http://arduino.googlecode.com/svn/trunk/libraries/LiquidCrystal/
 *
 * I converted the C++ version into C; so, no objects, constructors and the such
 *
 * Port B (PB0..2) is used for control
 * Port A (PA0..3) is used for data. Note: only 4 bit data communication is used
 */ 


#ifndef LCD_AT84_C_H_
#define LCD_AT84_C_H_


	#define DDR_CTL DDRB
	#define PORT_CTL PORTB
	#define _PIN_RS PB0
	#define _PIN_RW PB1
	#define _PIN_EN PB2
	// only higher nibble of lcd dataport is used: DB4 .. DB7
	// DB7 is also a busyflag, to be read
	#define DDR_DATA DDRA
	#define PORT_DATA PORTA
	#define _DB4 PA0
	#define _DB5 PA1
	#define _DB6 PA2
	#define _DB7 PA3

	// commands
	#define LCD_CLEARDISPLAY 0x01
	#define LCD_RETURNHOME 0x02
	#define LCD_ENTRYMODESET 0x04
	#define LCD_DISPLAYCONTROL 0x08
	#define LCD_CURSORSHIFT 0x10
	#define LCD_FUNCTIONSET 0x20
	#define LCD_SETCGRAMADDR 0x40
	#define LCD_SETDDRAMADDR 0x80

	// flags for display entry mode
	#define LCD_ENTRYRIGHT 0x00
	#define LCD_ENTRYLEFT 0x02
	#define LCD_ENTRYSHIFTINCREMENT 0x01
	#define LCD_ENTRYSHIFTDECREMENT 0x00

	// flags for display on/off control
	#define LCD_DISPLAYON 0x04
	#define LCD_DISPLAYOFF 0x00
	#define LCD_CURSORON 0x02
	#define LCD_CURSOROFF 0x00
	#define LCD_BLINKON 0x01
	#define LCD_BLINKOFF 0x00

	// flags for display/cursor shift
	#define LCD_DISPLAYMOVE 0x08
	#define LCD_CURSORMOVE 0x00
	#define LCD_MOVERIGHT 0x04
	#define LCD_MOVELEFT 0x00

	// flags for function set
	#define LCD_8BITMODE 0x10
	#define LCD_4BITMODE 0x00
	#define LCD_2LINE 0x08
	#define LCD_1LINE 0x00
	#define LCD_5x10DOTS 0x04
	#define LCD_5x8DOTS 0x00
	#define false 0x00
	#define true 0x01

		void LCD_init();
		
		void LCD_begin(uint8_t cols, uint8_t rows, uint8_t charsize);

		void LCD_clear();
		void LCD_home();

		void LCD_noDisplay();
		void LCD_display();
		void LCD_noBlink();
		void LCD_blink();
		void LCD_noCursor();
		void LCD_cursor();
		void LCD_scrollDisplayLeft();
		void LCD_scrollDisplayRight();
		void CDL_leftToRight();
		void LCD_rightToLeft();
		void LCD_autoscroll();
		void LCD_noAutoscroll();

		void LCD_setRowOffsets(int row1, int row2, int row3, int row4);
		void LCD_createChar(uint8_t, uint8_t[]);
		void LCD_setCursor(uint8_t, uint8_t);
//		virtual size_t write(uint8_t);
		void LCD_write(uint8_t);
		void LCD_command(uint8_t);
//		using Print::write;
		
		void LCD_send(uint8_t, uint8_t);
		void LCD_write4bits(uint8_t);
		void LCD_write8bits(uint8_t);
		void LCD_pulseEnable();

		uint8_t _rs_pin; // LOW: command.  HIGH: character.
		uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
		uint8_t _enable_pin; // activated by a HIGH pulse.
		uint8_t _data_pins[8];

		uint8_t _displayfunction;
		uint8_t _displaycontrol;
		uint8_t _displaymode;

		uint8_t _initialized;

		uint8_t _numlines;
		uint8_t _row_offsets[4];

#endif /* LCD_AT84_C_H_ */
