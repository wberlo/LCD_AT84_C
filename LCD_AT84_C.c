/*
 * LCD_AT84.c
 *
 * Created: 2015-04-18 20:27:05
 *  Author: Wim
 */ 
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>
#include "LCD_AT84_C.h"


void LCD_init()
{
	DDR_CTL |= (1<<_PIN_RS) | (1<<_PIN_RW) | (1<<_PIN_EN);
	
	
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	
}

void LCD_begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
	DDR_CTL |= (1<<_PIN_RS) | (1<<_PIN_RW) | (1<<_PIN_EN);
	
	
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (lines > 1) _displayfunction |= LCD_2LINE;
	
	_numlines = lines;

	LCD_setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != LCD_5x8DOTS) && (lines == 1)) _displayfunction |= LCD_5x10DOTS;

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands.
	_delay_us(50000);
	// Now we pull both RS and R/W low to begin commands
	PORT_CTL &= ~(1<<_PIN_RS);
	PORT_CTL &= ~(1<<_PIN_EN);
	PORT_CTL &= ~(1<<_PIN_RW);
	
	//put the LCD into 4 bit or 8 bit mode
	if (! (_displayfunction & LCD_8BITMODE))
	{
		// this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46

		// we start in 8bit mode, try to set 4 bit mode
		LCD_write4bits(0x03);
		_delay_us(4500); // wait min 4.1ms

		// second try
		LCD_write4bits(0x03);
		_delay_us(4500); // wait min 4.1ms
		
		// third go!
		LCD_write4bits(0x03);
		_delay_us(150);

		// finally, set to 4-bit interface
		LCD_write4bits(0x02);
	}
	else
	{
		// this is according to the hitachi HD44780 datasheet
		// page 45 figure 23

		// Send function set command sequence
		LCD_command(LCD_FUNCTIONSET | _displayfunction);
		_delay_us(4500);  // wait more than 4.1ms

		// second try
		LCD_command(LCD_FUNCTIONSET | _displayfunction);
		_delay_us(150);

		// third go
		LCD_command(LCD_FUNCTIONSET | _displayfunction);
	}

	// finally, set # lines, font size, etc.
	LCD_command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	LCD_display();

	// clear it off
	LCD_clear();

	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	LCD_command(LCD_ENTRYMODESET | _displaymode);

}

void LCD_setRowOffsets(int row0, int row1, int row2, int row3)
{
	_row_offsets[0] = row0;
	_row_offsets[1] = row1;
	_row_offsets[2] = row2;
	_row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void LCD_clear()
{
	LCD_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	_delay_us(2000);  // this LCD_command takes a long time!
}

void LCD_home()
{
	LCD_command(LCD_RETURNHOME);  // set cursor position to zero
	_delay_us(2000);  // this command takes a long time!
}

void LCD_setCursor(uint8_t col, uint8_t row)
{
	const uint8_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
	if ( row >= max_lines ) row = max_lines - 1;    // we count rows starting w/0
	if ( row >= _numlines ) row = _numlines - 1;    // we count rows starting w/0
	
	LCD_command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void LCD_noDisplay()
{
	_displaycontrol &= ~LCD_DISPLAYON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD_display()
{
	_displaycontrol |= LCD_DISPLAYON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LCD_noCursor()
{
	_displaycontrol &= ~LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD_cursor()
{
	_displaycontrol |= LCD_CURSORON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LCD_noBlink()
{
	_displaycontrol &= ~LCD_BLINKON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD_blink()
{
	_displaycontrol |= LCD_BLINKON;
	LCD_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LCD_scrollDisplayLeft(void)
{
	LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCD_scrollDisplayRight(void)
{
	LCD_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LCD_leftToRight(void)
{
	_displaymode |= LCD_ENTRYLEFT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LCD_rightToLeft(void)
{
	_displaymode &= ~LCD_ENTRYLEFT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LCD_autoscroll(void)
{
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LCD_noAutoscroll(void)
{
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	LCD_command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCD_createChar(uint8_t location, uint8_t charmap[])
{
	location &= 0x7; // we only have 8 locations 0-7
	LCD_command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		LCD_write(charmap[i]);
	}
}

/*********** mid level commands, for sending data/cmds */

void LCD_command(uint8_t value)
{
	LCD_send(value, false);
}

//virtual size_t LCD_write(uint8_t value)
void LCD_write(uint8_t value)
{
	LCD_send(value, true);
	//return(1);
}

/************ low level data pushing commands **********/

void LCD_send(uint8_t value, uint8_t mode)
{
	if (!mode)
	  PORT_CTL &= ~(1<<_PIN_RS);
	 else
	  PORT_CTL |= (1<<_PIN_RS);

	// if there is a RW pin indicated, set it low to Write
	  PORT_CTL &= ~(1<<_PIN_RW);
	
		LCD_write4bits(value>>4);
		LCD_write4bits(value);
}

void LCD_pulseEnable(void)
{
	PORT_CTL &= ~(1<<_PIN_EN);
	_delay_us(1);
	PORT_CTL |= (1<<_PIN_EN);
	_delay_us(1);    // enable pulse must be >450ns
	PORT_CTL &= ~(1<<_PIN_EN);
	_delay_us(100);   // commands need > 37us to settle
}

void LCD_write4bits(uint8_t datavalue)
{
	DDR_DATA |= (1<<_DB4) | (1<<_DB5) | (1<<_DB6) | (1<<_DB7);
	PORT_DATA &= ~((1<<_DB4) | (1<<_DB5) | (1<<_DB6) | (1<<_DB7));  // null the lower nibble
	
	PORT_DATA |= (datavalue & 0x0f); // write the lower nibble using a mask
	
	LCD_pulseEnable();
}
