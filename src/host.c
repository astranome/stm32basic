/*
host.c file is a part of stm32Basic project.

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
#include <math.h>
#include <stdint.h>
#include "../include/ps2kbd.h"
#include "../include/host.h"
#include "../include/utility.h"
#include "../include/basic.h"
#include "../include/lcd2004.h"
#include "../include/term_io.h"
#include "../include/rtc.h"
#include "../chan_fatfs/src/ff.h"
#include "../chan_fatfs/src/diskio.h"

/* Global variables */
char screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
char lineDirty[SCREEN_HEIGHT];
int curX = 0, curY = 0;
volatile char flash = 1, redraw = 0;
char inputMode = 0;
char inkeyChar = 0;
#ifdef BUZZER_IN_USE
char buzPin = 0
#endif

#ifdef SD_CARD_IN_USE
/* bool sd_card_ok = false; TODO */
#endif

const char bytesFreeStr[] = "bytes free";

void host_init(int buzzerPin)
{
    buzzerPin = buzzerPin; /* TODO */

#ifdef BUZZER_IN_USE
    /* buzPin = buzzerPin; TODO */
#endif

    clock_setup();

#ifdef SERIAL_TRACES_ENABLED
    usart_setup();
    host_sleep(100);
#endif

    i2c_setup();
    lcd2004_setup();
    lcd2004_init_4bit_mode();
    lcd2004_clear();
    lcd2004_home();
    lcd2004_backlight_off();

    ext_interrupt_setup();
    misc_gpio_setup();
    kbd_begin();

    DEBUG_SERIAL_PRINT(gimmick);

#ifdef BUZZER_IN_USE
    if (buzPin)
        /* pinMode(buzPin, OUTPUT); TODO */
#endif
}

void host_sleep(long ms)
{
    delay_us100(ms * 10);
}

void host_digitalWrite(int pin, int state)
{
    /* TODO */
    pin = pin;
    state = state;
}

int host_digitalRead(int pin)
{
    pin = pin;
    return 0;       /* TODO*/
}

int host_analogRead(int pin)
{
    pin = pin;
    return 0;       /* TODO */
}

void host_pinMode(int pin,int mode)
{
    /* TODO */
    pin = pin;
    mode = mode;
}

#ifdef BUZZER_IN_USE
void host_click()
{
/* TODO
    if (!buzPin) return;
*/
}

void host_startupTone()
{
    if (!buzPin) return;

    for (int i=1; i<=2; i++)
    {
        for (int j=0; j<50*i; j++)
        {
            digitalWrite(buzPin, HIGH);
            delay(3-i);
            digitalWrite(buzPin, LOW);
            delay(3-i);
        }

        delay(100);
    }
}
#endif

void host_cls()
{
    memset(screenBuffer, 32, SCREEN_WIDTH * SCREEN_HEIGHT);
    memset(lineDirty, 1, SCREEN_HEIGHT);
    curX = 0;
    curY = 0;
}

void host_moveCursor(int x, int y)
{
    if (x < 0)
        x = 0;

    if (x >= SCREEN_WIDTH)
        x = SCREEN_WIDTH - 1;

    if (y < 0)
        y = 0;

    if (y >= SCREEN_HEIGHT)
        y = SCREEN_HEIGHT - 1;

    curX = x;
    curY = y;
}

void host_showBuffer()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        if (lineDirty[y] || (inputMode && y == curY))
        {
            lcd2004_set_cursor(0, y);

            for (int x = 0; x < SCREEN_WIDTH; x++)
            {
                char c = screenBuffer[y * SCREEN_WIDTH + x];
                if (c < 32)
                {
                    c = ' ';
                }

                if (x == curX && y == curY && inputMode && flash)
                {
                    c = CURSOR_CHR;
                }

                lcd2004_write_character_4d(c);

            }

            lineDirty[y] = 0;
        }
    }
}

void scroll_buffer(void)
{
    memcpy(screenBuffer, screenBuffer + SCREEN_WIDTH, SCREEN_WIDTH * (SCREEN_HEIGHT - 1));
    memset(screenBuffer + SCREEN_WIDTH * (SCREEN_HEIGHT - 1), 32, SCREEN_WIDTH);
    memset(lineDirty, 1, SCREEN_HEIGHT);
    curY--;
}

void host_outputString(char *str)
{
    int pos = curY * SCREEN_WIDTH + curX;
    while (*str)
    {
        lineDirty[pos / SCREEN_WIDTH] = 1;
        screenBuffer[pos++] = *str++;
        if (pos >= SCREEN_WIDTH * SCREEN_HEIGHT)
        {
            scroll_buffer();
            pos -= SCREEN_WIDTH;
        }
    }

    curX = pos % SCREEN_WIDTH;
    curY = pos / SCREEN_WIDTH;
}

void host_outputProgMemString(const char *p)
{
    while (1)
    {
        unsigned char c = *(p++);
        if (c == 0)
            break;

        host_outputChar(c);
    }
}

void host_outputChar(char c)
{
    int pos = curY * SCREEN_WIDTH + curX;
    lineDirty[pos / SCREEN_WIDTH] = 1;

    screenBuffer[pos++] = c;
    if (pos >= SCREEN_WIDTH * SCREEN_HEIGHT)
    {
        scroll_buffer();
        pos -= SCREEN_WIDTH;
    }

    curX = pos % SCREEN_WIDTH;
    curY = pos / SCREEN_WIDTH;
}

int host_outputInt(long num)
{
    /* Returns len */
    long i = num, xx = 1;
    int c = 0;
    do {
        c++;
        xx *= 10;
        i /= 10;
    }
    while (i);

    for (int z = 0; z < c; z++)
    {
        xx /= 10;
        char digit = ((num/xx) % 10) + '0';
        host_outputChar(digit);
    }

    return c;
}

char *host_floatToStr(float f, char *buf)
{
    /* Floats have approx 7 sig figs */
    float a = (double)fabs(f);

    if (f == 0.0f)
    {
        buf[0] = '0';
        buf[1] = 0;
    }
    else if (a < 0.0001 || a > 1000000)
    {
#if 1 /* TODO */
        sprintf(buf, "%f", f);
#else
        /* This will output -1.123456E99 = 13 characters max including trailing nul */
        dtostre(f, buf, 6, 0);
#endif
    }
    else
    {
        int decPos = 7 - (int)(floor(log10(a))+1.0f);

#if 1 /* TODO */
        sprintf(buf, "%f", f);
#else
        dtostrf(f, 1, decPos, buf);
#endif

        if (decPos)
        {
            /* Remove trailing 0s */
            char *p = buf;
            while (*p) p++;
            p--;
            while (*p == '0')
            {
                *p-- = 0;
            }

            if (*p == '.')
                *p = 0;
        }
    }

    return buf;
}

void host_outputFloat(float f)
{
    char buf[16];
    host_outputString(host_floatToStr(f, buf));
}

void host_newLine()
{
    curX = 0;
    curY++;

    if (curY == SCREEN_HEIGHT)
        scroll_buffer();

    memset(screenBuffer + SCREEN_WIDTH * (curY), 32, SCREEN_WIDTH);
    lineDirty[curY] = 1;
}

char *host_readLine()
{
    inputMode = 1;

    if (curX == 0)
    {
        memset(screenBuffer + SCREEN_WIDTH*(curY), 32, SCREEN_WIDTH);
    }
    else
    {
        host_newLine();
    }

    int startPos = curY*SCREEN_WIDTH+curX;
    int pos = startPos;
    char c = 0;

    bool done = false;
    while (!done)
    {
        while (kbd_available())
        {
#ifdef BUZZER_IN_USE
            host_click();
#endif

            // read the next key
            lineDirty[pos / SCREEN_WIDTH] = 1;

            c = (char)kbd_read();

            if (c == PS2_F7)
            {
                lcd2004_backlight_toggle();
            }

            if (c >= 32 && c <= 126)
            {
                screenBuffer[pos++] = c;
            }
            else if (c == PS2_BACKSPACE && pos > startPos)
            {
                screenBuffer[--pos] = 0;
            }
            else if (c == PS2_ENTER)
            {
                done = true;
            }

            curX = pos % SCREEN_WIDTH;
            curY = pos / SCREEN_WIDTH;

            // scroll if we need to
            if (curY == SCREEN_HEIGHT)
            {
                if (startPos >= SCREEN_WIDTH)
                {
                    startPos -= SCREEN_WIDTH;
                    pos -= SCREEN_WIDTH;
                    scroll_buffer();
                }
                else
                {
                    screenBuffer[--pos] = 0;
                    curX = pos % SCREEN_WIDTH;
                    curY = pos / SCREEN_WIDTH;
                }
            }

            redraw = 1;
        }

        if (redraw)
        {
            host_showBuffer();
        }
    }

    screenBuffer[pos] = 0;
    inputMode = 0;

    // remove the cursor
    lineDirty[curY] = 1;
    host_showBuffer();
    return &screenBuffer[startPos];
}

char host_getKey()
{
    char c = inkeyChar;
    inkeyChar = 0;

    if (c >= 32 && c <= 126)
        return c;
    else
        return 0;
}

uint8_t host_ESCPressed()
{
    while (kbd_available())
    {
        // read the next key
        inkeyChar = kbd_read();
        if (inkeyChar == PS2_ESC)
        {
            return true;
        }
    }

    return false;
}

void host_outputFreeMem(unsigned int val)
{
    host_newLine();
    host_outputInt(val);
    host_outputChar(' ');
    host_outputProgMemString(bytesFreeStr);
}

void host_saveProgram(uint8_t autoexec)
{
    autoexec = autoexec;
#if 0 /* TODO */
    EEPROM.write(0, autoexec ? MAGIC_AUTORUN_NUMBER : 0x00);
    EEPROM.write(1, sysPROGEND & 0xFF);
    EEPROM.write(2, (sysPROGEND >> 8) & 0xFF);

    for (int i = 0; i < sysPROGEND; i++)
        EEPROM.write(3 + i, mem[i]);
#endif
}

void host_loadProgram()
{
#if 0 /* TODO */
    sysPROGEND = EEPROM.read(1) | (EEPROM.read(2) << 8);

    for (int i=0; i<sysPROGEND; i++)
        mem[i] = EEPROM.read(i+3);
#endif
}

#ifdef SD_CARD_IN_USE
bool host_saveSdCard(char *fileName)
{
    FIL dataFile;
    FRESULT rc;
    FATFS Fatfs[_VOLUMES];      /* File system object for each logical drive */
    UINT s1, s2;
    bool ret = true;
    unsigned int fileNameLen = strlen(fileName);
    char buf[16];

    if(fileNameLen > 8)
    {
        DEBUG_SERIAL_PRINT("File name is too long:%d!", fileNameLen);
        return false;
    }

    sprintf(buf, "%s.BAS", fileName);

    DEBUG_SERIAL_PRINT("Save FileName:%s, Len:%d, progLen:%d",
        buf,
        (int)fileNameLen,
        (int)sysPROGEND);

    // Mount logical drive
    rc = f_mount(0, &Fatfs[0]);
    if (rc != FR_OK)
    {
        DEBUG_SERIAL_PRINT("SD card mount error:%d", rc);
        return false;
    }
    else
    {
        DEBUG_SERIAL_PRINT("SD card mount OK");
    }

    // Open file for writing
    if (rc == FR_OK)
    {
        rc = f_open(&dataFile, buf, FA_WRITE | FA_CREATE_ALWAYS);
        if (rc != FR_OK)
        {
            DEBUG_SERIAL_PRINT("File open error:%d", rc);
            ret = false;
        }
        else
        {
            DEBUG_SERIAL_PRINT("File open OK");
        }
    }
    
    // Write data to the file
    if (rc == FR_OK)
    {
        s1 = sysPROGEND;
        rc = f_write(&dataFile, mem, s1, &s2);
        if (rc != FR_OK)
        {
            DEBUG_SERIAL_PRINT("File write error:%d", rc);
            ret = false;
        }
        else
        {
            DEBUG_SERIAL_PRINT("%d bytes written", s2);
        }
    }
	
    // Close the file
    if (rc == FR_OK)
    {
        rc = f_close(&dataFile);
        if (rc != FR_OK)
        {
            DEBUG_SERIAL_PRINT("File close error:%d", rc);
            ret = false;
        }
        else
        {
            DEBUG_SERIAL_PRINT("File close OK");
        }
    }

    // Unmount Logical Drive
    rc = f_mount(0, NULL);
    if (rc != FR_OK)
    {
        DEBUG_SERIAL_PRINT("SD card unmount error:%d", rc);
        ret = false;
    }
    else
    {
        DEBUG_SERIAL_PRINT("SD card unmount OK");
    }

    if(ret)
    {
        host_outputString("OK");
    }

    return ret;
}


bool host_loadSdCard(char *fileName)
{
    FIL dataFile;
    FRESULT rc;
    FATFS Fatfs[_VOLUMES];      /* File system object for each logical drive */
    bool ret = true;
    UINT cnt;
    int fileSize = 0;
    char buf[16];

    sprintf(buf, "%s.BAS", fileName);

    DEBUG_SERIAL_PRINT("Load FileName:%s", buf);

   // Mount logical drive
    rc = f_mount(0, &Fatfs[0]);
    if (rc != FR_OK)
    {
        DEBUG_SERIAL_PRINT("SD card mount error:%d", rc);
        return false;
    }
    else
    {
        DEBUG_SERIAL_PRINT("SD card mount OK");
    }

    // Open file for reading
    if (rc == FR_OK)
    {
        rc = f_open(&dataFile, buf, FA_READ);
        if (rc != FR_OK)
        {
            DEBUG_SERIAL_PRINT("File open error:%d", rc);
            ret = false;
        }
        else
        {
            DEBUG_SERIAL_PRINT("File open OK");
        }
    }

    // Get size of the file
    if (rc == FR_OK)
    {
        fileSize = dataFile.fsize;
        DEBUG_SERIAL_PRINT("File size:%d", fileSize);

        sysPROGEND = fileSize;
        rc = f_read(&dataFile, mem, fileSize, &cnt);
        if (rc != FR_OK)
        {
            DEBUG_SERIAL_PRINT("File read error:%d", rc);
            ret = false;
        }
        else
        {
            DEBUG_SERIAL_PRINT("File read OK, bytes:%d", cnt);
        }
    }

    // Close the file
    if (rc == FR_OK)
    {
        rc = f_close(&dataFile);
        if (rc != FR_OK)
        {
            DEBUG_SERIAL_PRINT("File close error:%d", rc);
            ret = false;
        }
        else
        {
            DEBUG_SERIAL_PRINT("File close OK");
        }
    }

    // Unmount Logical Drive
    rc = f_mount(0, NULL);
    if (rc != FR_OK)
    {
        DEBUG_SERIAL_PRINT("SD card unmount error:%d", rc);
        ret = false;
    }
    else
    {
        DEBUG_SERIAL_PRINT("SD card unmount OK");
    }

    if(ret)
    {
        host_outputString("OK");
    }

    return ret;
}
#endif

