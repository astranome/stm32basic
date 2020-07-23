/*----------------------------------------------------------------------------/
/ Based on libopencm3 library
/ https://github.com/libopencm3/libopencm3
/
/ kbd_test.c module is a part of Stm32Basic for stm32 systems.
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
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#ifdef SERIAL_TRACES_ENABLED
#include <libopencm3/stm32/usart.h>
#endif

#include "../include/utility.h"
#include "../include/ps2kbd.h"
#include "../include/lcd.h"

const char gimmick[] =
"         __           ________   ____             _     \r\n"\
"   _____/ /_____ ___ |__  /__ \\ / __ )____ ______(_)____\r\n"\
"  / ___/ __/ __ `__ \\ /_ <__/ // __  / __ `/ ___/ / ___/\r\n"\
" (__  ) /_/ / / / / /__/ / __// /_/ / /_/ (__  ) / /__  \r\n"\
"/____/\\__/_/ /_/ /_/____/____/_____/\\__,_/____/_/\\___/\r\n";

const char ver[] = "KBD tester, v.0.23";

void dump_keyboard_data(char c, char *s);

int main(void)
{
    int t;
    char c;
    bool in_loop = true;
    clock_setup();

#ifdef SERIAL_TRACES_ENABLED
    usart_setup();
#endif

    comm_puts(gimmick);
    comm_puts(ver);
    xprintf("\r\n");

    lcd_setup();
    lcd_init_4bit_mode();
    lcd_backlight_on();
    lcd_clear();
    lcd_home();
    lcd_backlight_on();
    
    ext_interrupt_setup();
    misc_gpio_setup();
    kbd_begin();
    
    lcd_clear();
    lcd_home();
    lcd_write_string_4d(ver);
    
    while(in_loop)
    {
        if (kbd_available())
        {
            /* Read the next key */
            t = kbd_read();
            
            if(t >= 0)
            {
                c = (char)t;

                lcd_clear();
                lcd_home();

                /* Check for some of the special keys */
                if (c == PS2_ENTER)
                {
                    dump_keyboard_data(c, "[Enter]");
                }
                else if (c == PS2_TAB)
                {
                    dump_keyboard_data(c, "[Tab]");
                }
                else if (c == PS2_ESC)
                {
                    dump_keyboard_data(c, "[ESC]");
                }
                else if (c == PS2_PAGEDOWN)
                {
                    dump_keyboard_data(c, "[PgDn]");
                }
                else if (c == PS2_PAGEUP)
                {
                    dump_keyboard_data(c, "[PgUp]");
                }
                else if (c == PS2_LEFTARROW)
                {
                    dump_keyboard_data(c, "[Left]");
                }
                else if (c == PS2_RIGHTARROW)
                {
                    dump_keyboard_data(c, "[Right]");
                }
                else if (c == PS2_UPARROW)
                {
                    dump_keyboard_data(c, "[Up]");
                }
                else if (c == PS2_DOWNARROW)
                {
                    dump_keyboard_data(c, "[Down]");
                }
                else if (c == PS2_DELETE)
                {
                    dump_keyboard_data(c, "[Del]");
                }
                else if (c == PS2_BACKSPACE)
                {
                    dump_keyboard_data(c, "[BackSpace]");
                }
                else if (c == ' ')
                {
                    dump_keyboard_data(c, "[Space]");
                }
                else if (c == PS2_F7)
                {
                    dump_keyboard_data(c, "[F7]");
                }
                else
                {
                    dump_keyboard_data(c, "[]");
                }
            }
        }
    }

    return 0;
}

void dump_keyboard_data(char c, char *s)
{
    char buf[20];

    if (c > 31 && c < 127)
    {
        sprintf (buf, "%d:%c,%s", c, c, s);
    }
    else
    {
        sprintf (buf, "%d:%s", c, s);
    }
    
    lcd_write_string_4d(buf);
    DEBUG_PRINT(buf);
}