/*
tft320240_test.c file is a part of stm32Basic project.

Copyright (c) 2020 vitasam

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
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#ifdef SERIAL_TRACES_ENABLED
#include <libopencm3/stm32/usart.h>
#endif

#include "../include/utility.h"
#include "../include/tft320240.h"

DisplayCapability dispCapability;
const char applicationName[] = "TFT 320x240 tester";
const char teststr[] = "stm32basic";

int main(void) {
    clock_setup();

    usart_setup();

    comm_puts(gimmick);
    comm_puts(globalVer);
    comm_puts(newL);
    comm_puts(applicationName);
    comm_puts(newL);

    tft320240_get_capability(&dispCapability);
    tft320240_init();

    DEBUG_SERIAL_PRINT("Display name: %s", dispCapability.displayName);
    DEBUG_SERIAL_PRINT("Width, pixels: %d", dispCapability.displayWidthPixels);
    DEBUG_SERIAL_PRINT("Height, pixels: %d", dispCapability.displayHeightPixels);
    DEBUG_SERIAL_PRINT("Width, symbols: %d", dispCapability.displayWidthSymbols);
    DEBUG_SERIAL_PRINT("Height, symbols: %d", dispCapability.displayHeightSymbols);
    DEBUG_SERIAL_PRINT("Has backlight: %d", dispCapability.displayHasBacklight);

    delay_us100(SEC_2);
    DEBUG_SERIAL_PRINT("=== Test of TFT cursor");

    char testBuf[24];
    int i;
    for (i = 0; i < 15; i++) {
        sprintf(testBuf, "Curs:%d,%d %s", i, i + 10, teststr);
        DEBUG_SERIAL_PRINT("%s", testBuf);
        tft320240_set_cursor(i, i + 10);
        tft8bit_write_string(testBuf);
    }

/*
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
 */   
    DEBUG_SERIAL_PRINT("Ok\r\n");
    return 0;
}
