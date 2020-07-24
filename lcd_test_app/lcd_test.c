/*----------------------------------------------------------------------------/
/ Based on libopencm3 library
/ https://github.com/libopencm3/libopencm3
/
/ lcd_test.c module is a part of Stm32Basic for stm32 systems.
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
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#ifdef SERIAL_TRACES_ENABLED
#include <libopencm3/stm32/usart.h>
#endif

#include "../include/utility.h"
#include "../include/lcd.h"

const char applicationName[] = "LCD tester";
const char teststr[] = " Stm32Basic";

int main(void)
{
    clock_setup();

#ifdef SERIAL_TRACES_ENABLED
    usart_setup();
#endif

    lcd_setup();

    comm_puts(gimmick);
    comm_puts(globalVer);
    comm_puts(newLine);
    comm_puts(applicationName);
    comm_puts(newLine);
    
    lcd_init_4bit_mode();
    lcd_backlight_on();
    lcd_clear();
    lcd_home();
    lcd_write_string_4d(applicationName);
    lcd_set_cursor(0, 1);
    lcd_write_string_4d(globalVer);
    lcd_set_cursor(0, 2);
    delay_us100(SEC_2);

    lcd_off();
    DEBUG_PRINT("=== LCD off");
    delay_us100(SEC_2);

    lcd_on();
    DEBUG_PRINT("=== LCD on");
    delay_us100(SEC_2);
    lcd_clear();
    lcd_home();
    
    DEBUG_PRINT("=== Test of LCD cursor:");

    lcd_set_cursor(0, 0);
    lcd_write_string_4d("@0,0");
    lcd_write_string_4d(teststr);
    DEBUG_PRINT("@0,0");
    DEBUG_PRINT(teststr);
    delay_us100(SEC_2); 

    lcd_set_cursor(1, 1);
    lcd_write_string_4d("@1,1");
    lcd_write_string_4d(teststr);
    DEBUG_PRINT("@1,1");
    DEBUG_PRINT(teststr);
    delay_us100(SEC_2); 

    lcd_set_cursor(2, 2);
    lcd_write_string_4d("@2,2");
    lcd_write_string_4d(teststr);
    DEBUG_PRINT("@2,2");
    DEBUG_PRINT(teststr);
    delay_us100(SEC_2); 

    lcd_set_cursor(3, 3);
    lcd_write_string_4d("@3,3");
    lcd_write_string_4d(teststr);
    DEBUG_PRINT("@3,3");
    DEBUG_PRINT(teststr);
    delay_us100(SEC_2);
    
    DEBUG_PRINT("=== LCD backlight ON");
    lcd_backlight_on();
    delay_us100(SEC_2);
    
    lcd_backlight_off();
    DEBUG_PRINT("=== LCD backlight OFF");
    delay_us100(SEC_2); 

    lcd_backlight_on();     
    DEBUG_PRINT("=== LCD backlight ON");
    delay_us100(SEC_2); 
    
    DEBUG_PRINT("Ok\r\n");
    return 0;
}
