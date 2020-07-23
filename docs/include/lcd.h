/*----------------------------------------------------------------------------/
/ lcd.h module is a part of Stm32Basic for stm32 systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following terms.
/
/ Copyright (C) 2020, Vitasam, all right reserved.
/
/ * The Stm32Basic is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/---------------------------------------------------------------------------*/
#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>

/* LCD instructions */
#define LCD_CLEAR               0x01        /* Replace all characters with ASCII 'space' */
#define LCD_HOME                0x02        /* Return cursor to first position on first line */
#define LCD_ENTRYMODE           0x06        /* Shift cursor from left to right on read/write */
#define LCD_DISPLAYOFF          0x08        /* Turn display off */
#define LCD_DISPLAYON           0x0C        /* Display on, cursor off, don't blink character */
#define LCD_FUNCTIONRESET       0x30        /* Reset the LCD */
#define LCD_FUNCTIONSET4B       0x28        /* 4-bit data, 2-line display, 5 x 7 font */
#define LCD_SETCURSOR           0x80        /* Set cursor position */

void lcd_init_4bit_mode(void);
void lcd_write_character_4d(char theCharacter);
void lcd_write_string_4d(const char *theString);
void lcd_clear(void);
void lcd_home(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_off(void);
void lcd_on(void);
void lcd_backlight_off(void);
void lcd_backlight_on(void);
void lcd_backlight_toggle(void);
#endif

