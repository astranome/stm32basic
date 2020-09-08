/*
kbd_test.c file is a part of stm32Basic project.

Copyright (c) 2020 vitasam

Based on Arduino BASIC
https://github.com/robinhedwards/ArduinoBASIC

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
#include <string.h>
#include <stdint.h>
#include "../include/lcd2004.h"
#include "../include/host.h"
#include "../include/basic.h"
#include "../include/utility.h"
#include "../chan_fatfs/src/ff.h"
#include "../chan_fatfs/src/diskio.h"

const char applicationName[] = "stm32basic HW 1.0";

uint8_t mem[MEMORY_SIZE];
uint8_t tokenBuf[TOKEN_BUF_SIZE];

int main(void)
{
    uint8_t in_loop = 1;

    reset();
    host_init(BUZZER_PIN);
    host_cls();

    host_outputProgMemString(applicationName);
    host_moveCursor(0, 1);
    host_outputProgMemString(globalVer);
    DEBUG_SERIAL_PRINT(applicationName);
    DEBUG_SERIAL_PRINT(globalVer);

#ifdef SD_CARD_IN_USE
#ifdef SERIAL_TRACES_ENABLED
    WORD res = (WORD)disk_initialize(0);
    DEBUG_SERIAL_PRINT("SD initialization: %s\n", res ? "FAIL" : "OK");
#else
    disk_initialize(0);
#endif
#endif

    // Show memory size
    host_outputFreeMem(sysVARSTART - sysPROGEND);
    host_showBuffer();

    while(in_loop)
    {
        int ret = ERROR_NONE;

        // Get a line from the user
        char *input = host_readLine();

        // Special editor commands
        if (input[0] == '?' && input[1] == 0)
        {
            host_outputFreeMem(sysVARSTART - sysPROGEND);
            host_showBuffer();
            return 0;
        }

        // Otherwise tokenize
        ret = tokenize((unsigned char*)input, tokenBuf, TOKEN_BUF_SIZE);

        // Execute the token buffer
        if (ret == ERROR_NONE)
        {
            host_newLine();
            ret = processInput(tokenBuf);
        }

        if (ret != ERROR_NONE)
        {
            host_newLine();
            if (lineNumber != 0)
            {
                host_outputInt(lineNumber);
                host_outputChar('-');
            }

            host_outputProgMemString((char *)pgm_read_word(&(errorTable[ret])));
        }
    }

    return 0;
}
