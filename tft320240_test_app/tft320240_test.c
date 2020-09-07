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

    char testBuf[48];
	int i, k;
	
    for (i = 0; i < 19; i++) {
		sprintf(testBuf, "Cursor:%d,%d", i, i);
		tft320240_set_cursor(i, i);
		char c = testBuf[0];
		k = 0;
		do
		{
			tft320240_set_cursor(i + k, i);
			tft320240_write_character(c);
			k++;
		} while ((c = testBuf[k]) != '\0');
	}

    delay_us100(SEC_2);
    DEBUG_SERIAL_PRINT("=== Test of Screen Size");

    tft320240_init();

    int xpos = 0;
	for (i = 0; i < 10; i++) {
		tft320240_set_cursor(xpos, 0);
		xpos++;
		tft320240_write_character(i + '0');
	}

	for (i = 0; i < 10; i++) {
		tft320240_set_cursor(xpos, 0);
		xpos++;
		tft320240_write_character(i + '0');
	}

	for (i = 0; i < 10; i++) {
		tft320240_set_cursor(xpos, 0);
		xpos++;
		tft320240_write_character(i + '0');
	}

	for (i = 0; i < 10; i++) {
		tft320240_set_cursor(xpos, 0);
		xpos++;
		tft320240_write_character(i + '0');
	}

	int ypos = 0;
	for (i = 0; i < 10; i++) {
		tft320240_set_cursor(0, ypos);
		ypos++;
		tft320240_write_character(i + '0');
	}

	for (i = 0; i < 10; i++) {
		tft320240_set_cursor(0, ypos);
		ypos++;
		tft320240_write_character(i + '0');
	}

	sprintf(testBuf, "Screen width: %d symbols", xpos);
	tft320240_set_cursor(8, 9);
	tft320240_write_string(testBuf, ILI_COLOR_RED);

	sprintf(testBuf, "Screen height:%d symbols", ypos);
	tft320240_set_cursor(8, 10);
	tft320240_write_string(testBuf, ILI_COLOR_RED);

    delay_us100(SEC_2);
    DEBUG_SERIAL_PRINT("Ok\r\n");
    return 0;
}
