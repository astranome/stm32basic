/*
 * lcd2004_test.c module is a part of stm32Basic project.
 *
 * Copyright (c) 2020 vitasam
 *
 * Based on libopencm3 library:
 *     https://github.com/libopencm3/libopencm3
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
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#ifdef SERIAL_TRACES_ENABLED
#include <libopencm3/stm32/usart.h>
#endif

#include "../include/utility.h"
#include "../include/lcd2004.h"

const char applicationName[] = "LCD tester";
const char teststr[] = " Stm32Basic";

int main() {
    clock_setup();

#ifdef SERIAL_TRACES_ENABLED
    usart_setup();
#endif

    DEBUG_SERIAL_PRINT(gimmick);
    DEBUG_SERIAL_PRINT(globalVer);
    DEBUG_SERIAL_PRINT(newL);
    DEBUG_SERIAL_PRINT(applicationName);
    DEBUG_SERIAL_PRINT(newL);

    lcd2004_setup();    
    lcd2004_init_4bit_mode();
    lcd2004_backlight_on();
    lcd2004_clear();
    lcd2004_home();
    lcd2004_write_string_4d(applicationName);
    lcd2004_set_cursor(0, 1);
    lcd2004_write_string_4d(globalVer);
    lcd2004_set_cursor(0, 2);
    delay_us100(SEC_2);

    lcd2004_off();
    DEBUG_SERIAL_PRINT("=== LCD off");
    delay_us100(SEC_2);

    lcd2004_on();
    DEBUG_SERIAL_PRINT("=== LCD on");
    delay_us100(SEC_2);
    lcd2004_clear();
    lcd2004_home();
    
    DEBUG_SERIAL_PRINT("=== Test of LCD cursor:");

    lcd2004_set_cursor(0, 0);
    lcd2004_write_string_4d("@0,0");
    lcd2004_write_string_4d(teststr);
    DEBUG_SERIAL_PRINT("@0,0");
    DEBUG_SERIAL_PRINT(teststr);
    delay_us100(SEC_2); 

    lcd2004_set_cursor(1, 1);
    lcd2004_write_string_4d("@1,1");
    lcd2004_write_string_4d(teststr);
    DEBUG_SERIAL_PRINT("@1,1");
    DEBUG_SERIAL_PRINT(teststr);
    delay_us100(SEC_2); 

    lcd2004_set_cursor(2, 2);
    lcd2004_write_string_4d("@2,2");
    lcd2004_write_string_4d(teststr);
    DEBUG_SERIAL_PRINT("@2,2");
    DEBUG_SERIAL_PRINT(teststr);
    delay_us100(SEC_2); 

    lcd2004_set_cursor(3, 3);
    lcd2004_write_string_4d("@3,3");
    lcd2004_write_string_4d(teststr);
    DEBUG_SERIAL_PRINT("@3,3");
    DEBUG_SERIAL_PRINT(teststr);
    delay_us100(SEC_2);
    
    DEBUG_SERIAL_PRINT("=== LCD backlight ON");
    lcd2004_backlight_on();
    delay_us100(SEC_2);
    
    lcd2004_backlight_off();
    DEBUG_SERIAL_PRINT("=== LCD backlight OFF");
    delay_us100(SEC_2); 

    lcd2004_backlight_on();     
    DEBUG_SERIAL_PRINT("=== LCD backlight ON");
    delay_us100(SEC_2); 
    
    DEBUG_SERIAL_PRINT("Ok\r\n");
    return 0;
}
