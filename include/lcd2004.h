/*
 * lcd2004.h module is a part of stm32Basic project.
 *
 * Copyright (c) 2020 vitasam
 * 
 * Based on LCD driver:
 * http://web.alfredstate.edu/faculty/weimandn/programming/
 *     lcd/ATmega328/LCD_code_gcc_4d.html
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _LCD_2004_H_
#define _LCD_2004_H_

#include <stdint.h>

/* LCD instructions */
#define LCD_CLEAR 0x01 /* Replace all characters with ASCII 'space' */
#define LCD_HOME 0x02 /* Return cursor to first position on first line */
#define LCD_ENTRYMODE 0x06 /* Shift cursor from left to right on read/write */
#define LCD_DISPLAYOFF 0x08 /* Turn display off */
#define LCD_DISPLAYON 0x0C /* Display on, cursor off, don't blink character */
#define LCD_FUNCTIONRESET 0x30 /* Reset the LCD */
#define LCD_FUNCTIONSET4B 0x28 /* 4-bit data, 2-line display, 5 x 7 font */
#define LCD_SETCURSOR 0x80 /* Set cursor position */

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
