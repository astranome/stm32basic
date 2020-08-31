/*
lcd2004.h file is a part of stm32Basic project.

Copyright (c) 2020 vitasam

Based on LCD driver:
http://web.alfredstate.edu/faculty/weimandn/programming/
    lcd/ATmega328/LCD_code_gcc_4d.html

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _LCD_2004_H_
#define _LCD_2004_H_

#include <stdint.h>

/* LCD pins */
#define LCD_PORT        GPIOB
#define LCD_E           GPIO10
#define LCD_RS          GPIO11
#define LCD_D4          GPIO12
#define LCD_D5          GPIO13
#define LCD_D6          GPIO14
#define LCD_D7          GPIO15
#define LCD_BACKLIGHT   GPIO1

/* LCD instructions */
#define LCD_CLEAR 0x01 /* Replace all characters with ASCII 'space' */
#define LCD_HOME 0x02 /* Return cursor to first position on first line */
#define LCD_ENTRYMODE 0x06 /* Shift cursor from left to right on read/write */
#define LCD_DISPLAYOFF 0x08 /* Turn display off */
#define LCD_DISPLAYON 0x0C /* Display on, cursor off, don't blink character */
#define LCD_FUNCTIONRESET 0x30 /* Reset the LCD */
#define LCD_FUNCTIONSET4B 0x28 /* 4-bit data, 2-line display, 5 x 7 font */
#define LCD_SETCURSOR 0x80 /* Set cursor position */

void lcd2004_init(void);
void lcd2004_init_gpio(void);
void lcd2004_init_4bit_mode(void);
void lcd2004_write_character_4d(char theCharacter);
void lcd2004_write_string_4d(const char *theString);
void lcd2004_clear(void);
void lcd2004_home(void);
void lcd2004_set_cursor(uint8_t col, uint8_t row);
void lcd2004_off(void);
void lcd2004_on(void);
void lcd2004_backlight_off(void);
void lcd2004_backlight_on(void);
void lcd2004_backlight_toggle(void);

#endif
