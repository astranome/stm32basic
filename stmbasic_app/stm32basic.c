/*----------------------------------------------------------------------------/
/ Based on Arduino BASIC
/ https://github.com/robinhedwards/ArduinoBASIC
/
/ stm32basic.c module is a part of Stm32Basic for stm32 systems.
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
#include <stdint.h>
#include "../include/lcd.h"
#include "../include/host.h"
#include "../include/basic.h"
#include "../include/utility.h"
#include "../chan_fatfs/src/ff.h"
#include "../chan_fatfs/src/diskio.h"

const char ver[] = "stm32f1BASIC v.0.70";

uint8_t mem[MEMORY_SIZE];
uint8_t tokenBuf[TOKEN_BUF_SIZE];

int main(void)
{
    uint8_t in_loop = 1;

    reset();
    host_init(BUZZER_PIN);
    host_cls();

    host_outputProgMemString(ver);
    DEBUG_PRINT(ver);

#ifdef SD_CARD_IN_USE
#ifdef SERIAL_TRACES_ENABLED
    WORD res = (WORD)disk_initialize(0);
    DEBUG_PRINT("SD initialization: %s\n", res ? "FAIL" : "OK");
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
