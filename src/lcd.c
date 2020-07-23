/*----------------------------------------------------------------------------/
/ Based on LCD driver
/ http://web.alfredstate.edu/faculty/weimandn/programming/lcd/ATmega328/LCD_code_gcc_4d.html
/
/ lcd.c module is a part of Stm32Basic for stm32 systems.
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
#include <stdio.h>
#include <string.h>
#include "../include/lcd.h"
#include "../include/utility.h"


/** @brief Send a byte of data to the LCD module
 *  (private function).
 *  @note RS is configured for the desired LCD register output to
 *  begin at the new position. If the cursor is
 *  currently hidden, a call to set_cursor() must not show
 *  the cursor. E is low. RW is low.
 *  @param theByte is a data to be sent to the desired LCD register.
 *  @return None.
 */
static void lcd_write_4(uint8_t theByte)
{
    if (theByte & (1 << 7))
    {
        gpio_set(GPIOB, LCD_D7);
    }
    else
    {
        gpio_clear(GPIOB, LCD_D7);
    }

    if (theByte & (1 << 6))
    {
        gpio_set(GPIOB, LCD_D6);
    }
    else
    {
        gpio_clear(GPIOB, LCD_D6);
    }

    if (theByte & (1 << 5))
    {
        gpio_set(GPIOB, LCD_D5);
    }
    else
    {
        gpio_clear(GPIOB, LCD_D5);
    }

    if (theByte & (1 << 4))
    {
        gpio_set(GPIOB, LCD_D4);
    }
    else
    {
        gpio_clear(GPIOB, LCD_D4);
    }

    /* Write the data. 'Address set-up time' (40 nS) */
    gpio_set(GPIOB, LCD_E);     /* Enable pin high */
    delay_us100(1);             /* 'Data set-up time' (80 nS) and 'Enable pulse width' (230 nS) */
    gpio_clear(GPIOB, LCD_E);   /* Enable pin low */
    delay_us100(1);             /* 'Data hold time' (10 nS) and 'Enable cycle time' (500 nS) */
}


/** @brief Send a byte of data to the LCD instruction register.
 *  (private function).
 *  @note Does not deal with RW (busy flag is not implemented).
 *  @param theByte is a data to be sent to LCD instruction register.
 *  @return None.
 */
static void lcd_write_instruction_4d(uint8_t theInstruction)
{
    gpio_clear(GPIOB, LCD_RS);          /* Select the Instruction Register (RS low) */
    gpio_clear(GPIOB, LCD_E);           /* Make sure E is initially low */
    lcd_write_4(theInstruction);        /* Write the upper 4-bits of the data */
    lcd_write_4(theInstruction << 4);   /* Write the lower 4-bits of the data */
}

/********************** Public functions **************************/

/** @brief Initialize the LCD module for a 4-bit data interface.
 *  @return None.
 */
void lcd_init_4bit_mode(void)
{
    /* Power-up delay, at least 40 msec */
    delay_us100(1000);          /* 100 msec */

    /*
        IMPORTANT - At this point the LCD module is in the 8-bit mode and it is
    expecting to receive 8 bits of data, one bit on each of its 8 data lines,
    each time the 'E' line is pulsed.

        Since the LCD module is wired for the 4-bit mode, only the upper four data
    lines are connected to the microprocessor and the lower four data lines are
    typically left open. Therefore, when the 'E' line is pulsed, the LCD controller
    will read whatever data has been set up on the upper four data lines and the
    lower four data lines will be high (due to internal pull-up circuitry).

        Fortunately the 'LCD_FUNCTIONRESET' instruction does not care about what is on
    the lower four bits so this instruction can be sent on just the four available
    data lines and it will be interpreted properly by the LCD controller.
    The 'lcd_write_4' subroutine will accomplish this if the control lines have
    previously been configured properly.
    */

    /* Set up the RS and E lines for the 'lcd_write_4' subroutine. */
    gpio_clear(GPIOB, LCD_RS);                  /* Select the Instruction Register (RS low) */
    gpio_clear(GPIOB, LCD_E);                   /* Make sure E is initially low */

    /* Reset the LCD controller */
    lcd_write_4(LCD_FUNCTIONRESET);             /* First part of reset sequence */
    delay_us100(60);                            /* 4.1 mS delay (min) */

    lcd_write_4(LCD_FUNCTIONRESET);             /* Second part of reset sequence */
    delay_us100(2);                             /* 100uS delay (min) */

    lcd_write_4(LCD_FUNCTIONRESET);             /* Third part of reset sequence */
    delay_us100(2);                             /* this delay is omitted in the data sheet */

    /*
        Preliminary Function Set instruction - used only to set the 4-bit mode.
    The number of lines or the font cannot be set at this time since the controller
    is still in the 8-bit mode, but the data transfer mode can be changed since this
    parameter is determined by one of the upper four bits of the instruction.
    */

    lcd_write_4(LCD_FUNCTIONSET4B);             /* Set 4-bit mode */
    delay_us100(1);                             /* 40uS delay (min) */

    /* Function Set instruction */
    lcd_write_instruction_4d(LCD_FUNCTIONSET4B); /* Set mode, lines, and font */
    delay_us100(1);                             /* 40uS delay (min) */

    /*
        The next three instructions are specified in the data sheet as part of
    the initialization routine, so it is a good idea (but probably not necessary) to
    do them just as specified and then redo them later if the application requires a
    different configuration.
    */

    /* Display On/Off Control instruction */
    lcd_write_instruction_4d(LCD_DISPLAYOFF);   /* Turn display OFF */
    delay_us100(1);                             /* 40uS delay (min) */

    /* Clear Display instruction */
    lcd_write_instruction_4d(LCD_CLEAR);        /* Clear display RAM */
    delay_us100(20);                            /* 1.64 mS delay (min) */

    /* Entry Mode Set instruction */
    lcd_write_instruction_4d(LCD_ENTRYMODE);    /* Set desired shift characteristics */
    delay_us100(1);                             /* 40uS delay (min) */

    /*
        This is the end of the LCD controller initialization as specified in
    the data sheet, but the display has been left in the OFF condition.
    This is a good time to turn the display back ON.
    */

    /* Display On/Off Control instruction */
    lcd_write_instruction_4d(LCD_DISPLAYON);    /* Turn the display ON */
    delay_us100(1);                             /* 40uS delay (min) */
}


/** @brief Send a byte of data to the LCD data register.
 *  @note Does not deal with RW (busy flag is not implemented).
 *  @param theCharacter is a data to be sent to LCD data register.
 *  @return None.
 */
void lcd_write_character_4d(char theCharacter)
{
    gpio_set(GPIOB, LCD_RS);                    /* Select the Data Register (RS high) */
    gpio_clear(GPIOB, LCD_E);                   /* Make sure E is initially low */
    lcd_write_4((uint8_t)theCharacter);         /* Write the upper 4-bits of the data */
    lcd_write_4((uint8_t)theCharacter << 4);    /* Write the lower 4-bits of the data */
}


/** @brief Display a string of characters on the LCD.
 *  @note Uses time delays rather than checking the busy flag.
 *  @param theString is the string to be displayed.
 *  @return None.
 */
void lcd_write_string_4d(const char *theString)
{
    volatile uint16_t i = 0;
    while (theString[i] != 0)
    {
        lcd_write_character_4d((uint8_t)theString[i]);
        i++;
        delay_us100(1);                             /* 40uS delay (min) */
    }
}

/** @brief Clear display RAM.
 *  @param None.
 *  @return None.
 */
void lcd_clear(void)
{
    lcd_write_instruction_4d(LCD_CLEAR);
    delay_us100(20);                            /* 1.64 mS delay (min) */
}

/** @brief Set cursor position to zero.
 *  @param None.
 *  @return None.
 */
void lcd_home(void)
{
    lcd_write_instruction_4d(LCD_HOME);
    delay_us100(20);                            /* 1.64 mS delay (min) */
}

/** @brief Set cursor position to Column and Row.
 *  @param col is start position of line of the LCD (0..19).
 *  @param row is start line of the LCD (0..3).
 *  @return None.
 */
void lcd_set_cursor(uint8_t col, uint8_t row)
{
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

    lcd_write_instruction_4d(LCD_SETCURSOR | (col + row_offsets[row]));
    delay_us100(20);                            /* 1.64 mS delay (min) */
}

/** @brief Set display OFF.
 *  @param None.
 *  @return None.
 */
void lcd_off(void)
{
    lcd_write_instruction_4d(LCD_DISPLAYOFF);
    delay_us100(20);                            /* 1.64 mS delay (min) */
}

/** @brief Set display ON.
 *  @param None.
 *  @return None.
 */
void lcd_on(void)
{
    lcd_write_instruction_4d(LCD_DISPLAYON);
    delay_us100(20);                            /* 1.64 mS delay (min) */
}

/** @brief Set display's backlight OFF.
 *  @param None.
 *  @return None.
 */
void lcd_backlight_off(void)
{
    gpio_clear(GPIOB, LCD_BACKLIGHT);
}

/** @brief Set display's backlight ON.
 *  @param None.
 *  @return None.
 */
void lcd_backlight_on(void)
{
    gpio_set(GPIOB, LCD_BACKLIGHT);
}

/** @brief Toggle display's backlight.
 *  @param None.
 *  @return None.
 */
void lcd_backlight_toggle(void)
{
    gpio_toggle(GPIOB, LCD_BACKLIGHT);
}