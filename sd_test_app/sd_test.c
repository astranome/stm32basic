/*
sd_test.c file is a part of stm32Basic project.

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
#include <string.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "../include/utility.h"
#include "../include/lcd2004.h"
#include "../include/rtc.h"
#include "../chan_fatfs/src/ff.h"
#include "../chan_fatfs/src/diskio.h"
#ifdef SERIAL_TRACES_ENABLED
#include <libopencm3/stm32/usart.h>
#endif

const char applicationName[] = "SD tester";

#define TEST_BUFFER_SIZE                4 * 1024

#ifdef SERIAL_TRACES_ENABLED
static void print_result_code(FRESULT rc) {
    const char *p;
    static const char str[] =
        "OK\0" "NOT_READY\0" "NO_FILE\0" "FR_NO_PATH\0" "INVALID_NAME\0" "INVALID_DRIVE\0"
        "DENIED\0" "EXIST\0" "RW_ERROR\0" "WRITE_PROTECTED\0" "NOT_ENABLED\0"
        "NO_FILESYSTEM\0" "INVALID_OBJECT\0" "MKFS_ABORTED\0";

    FRESULT i;
    for (p = str, i = 0; i != rc && *p; i++) {
        while(*p++);
    }

    xprintf("rc=%u FR_%s\n", (UINT)rc, p);
}
#endif

int main(void) {
    long p1, p2;
    BYTE testBuffer[TEST_BUFFER_SIZE] __attribute__ ((aligned (4)));
    UINT s1, s2, cnt;
    int res, w1;
    BYTE b1;
    FATFS Fatfs[_VOLUMES];      /* File system object for each logical drive */
    DIR Dir;                    /* Directory object */
    FRESULT rc;
    FILINFO FileInfo;
    FATFS *fs;                  /* Pointer to the file system object */
    FIL File1;
    DWORD ofs = 0;
    bool testPassed = true;

    clock_setup();
    usart_setup();
    lcd2004_setup();

    comm_puts(gimmick);
    comm_puts(globalVer);
    comm_puts(newL);
    comm_puts(applicationName);
    comm_puts(newL);

    lcd2004_init_4bit_mode();
    lcd2004_backlight_on();
    lcd2004_clear();
    lcd2004_home();
    lcd2004_write_string_4d(applicationName);
    lcd2004_set_cursor(0, 1);
    lcd2004_write_string_4d(globalVer);
    lcd2004_set_cursor(0, 2);
    lcd2004_write_string_4d("See results on UART");
    lcd2004_set_cursor(0, 3);

    /* Initialize disk */
    comm_puts("================== Init Disk:\r\n");
    res = (WORD)disk_initialize(0);
    xprintf("SD initialization: %s\n", res ? "FAIL" : "OK");
    if (res != RES_OK) {
        testPassed = false;
    }

    /* Show disk status */
    testBuffer[0] = 2;

    if (disk_ioctl(0, CTRL_POWER, testBuffer) == RES_OK) {
        xprintf("Power is %s\n", testBuffer[1] ? "ON" : "OFF");
    }
    else {
        testPassed = false;
    }

    if (disk_ioctl(0, GET_SECTOR_COUNT, &p2) == RES_OK) {
        xprintf("Drive size: %lu sectors\n", p2);
    }

    if (disk_ioctl(0, GET_SECTOR_SIZE, &w1) == RES_OK) {
        xprintf("Sector size: %u\n", w1);
    }

    if (disk_ioctl(0, GET_BLOCK_SIZE, &p2) == RES_OK) {
        xprintf("Erase block size: %lu sectors\n", p2);
    }

    if (disk_ioctl(0, MMC_GET_TYPE, &b1) == RES_OK) {
        xprintf("MMC/SDC type: %u\n", b1);
    }

    if (disk_ioctl(0, MMC_GET_CSD, testBuffer) == RES_OK) {
        xputs("CSD:\n"); put_dump(testBuffer, 0, 16);
    }

    if (disk_ioctl(0, MMC_GET_CID, testBuffer) == RES_OK) {
        xputs("CID:\n"); put_dump(testBuffer, 0, 16);
    }

    if (disk_ioctl(0, MMC_GET_OCR, testBuffer) == RES_OK) {
        xputs("OCR:\n"); put_dump(testBuffer, 0, 4);
    }

    if (disk_ioctl(0, MMC_GET_SDSTAT, testBuffer) == RES_OK) {
        xputs("SD Status:\n");
        for (s1 = 0; s1 < 64; s1 += 16) {
            put_dump(testBuffer + s1, s1, 16);
        }
    }

    comm_puts("================== Mount Logical Drive:\r\n");
    rc = f_mount(0, &Fatfs[0]);
    if (rc != FR_OK) {
        testPassed = false;
    }
    print_result_code(rc);

    comm_puts("================== Directory listing:\r\n");
    rc = f_opendir(&Dir, "");
    if (rc != FR_OK) {
        testPassed = false;
    }
    print_result_code(rc);

    p1 = s1 = s2 = 0;
    for(;;)
    {
        rc = f_readdir(&Dir, &FileInfo);
        if ((res != FR_OK) || !FileInfo.fname[0])
        {
            break;
        }

        if (FileInfo.fattrib & AM_DIR)
        {
            s2++;
        }
        else
        {
            s1++;
            p1 += FileInfo.fsize;
        }

        xprintf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s",
            (FileInfo.fattrib & AM_DIR) ? 'D' : '-',
            (FileInfo.fattrib & AM_RDO) ? 'R' : '-',
            (FileInfo.fattrib & AM_HID) ? 'H' : '-',
            (FileInfo.fattrib & AM_SYS) ? 'S' : '-',
            (FileInfo.fattrib & AM_ARC) ? 'A' : '-',
            (FileInfo.fdate >> 9) + 1980, (FileInfo.fdate >> 5) & 15, FileInfo.fdate & 31,
            (FileInfo.ftime >> 11), (FileInfo.ftime >> 5) & 63,
            FileInfo.fsize, &(FileInfo.fname[0]));

        xputc('\n');
    }

    xprintf("%4u File(s),%10lu bytes total\n%4u Dir(s)", s1, p1, s2);
    if (f_getfree("", (DWORD*)&p1, &fs) == FR_OK)
    {
        xprintf(", %10lu bytes free\n", p1 * fs->csize * 512);
    }

    comm_puts("================== Open (FA_READ) 'README.TXT' file:\r\n");
    rc = f_open(&File1, "README.TXT", FA_READ);
    print_result_code(rc);

    comm_puts("================== Read & Dump 200 bytes from 'README.TXT' file:\r\n");
    ofs = File1.fptr;
    p1 = 200;

    while (p1)
    {
        if ((UINT)p1 >= 16)
        {
            cnt = 16;
            p1 -= 16;
        }
        else
        {
            cnt = p1;
            p1 = 0;
        }

        rc = f_read(&File1, testBuffer, cnt, &cnt);
        
        if (res != FR_OK)
        {
            print_result_code(rc);
            break;
        }

        if (!cnt)
        {
            break;
        }

        put_dump(testBuffer, ofs, cnt);
        ofs += 16;
    }

    comm_puts("================== Close 'README.TXT' file:\r\n");
    rc = f_close(&File1);
    print_result_code(rc);

    comm_puts("================== Create (FA_WRITE | FA_CREATE_ALWAYS) 'WRITEME.TXT' file:\r\n");
    rc = f_open(&File1, "WRITEME.TXT", FA_WRITE | FA_CREATE_ALWAYS);
    if (rc != FR_OK) {
        testPassed = false;
    }
    print_result_code(rc);

    comm_puts("================== Write string to 'WRITEME.TXT' file:\r\n");
    memset(testBuffer, 0, TEST_BUFFER_SIZE);
    char hello[] = {"Hello from stm32Basic!"};
    cnt = sizeof(hello);
    rc = f_write(&File1, hello, cnt, &s2);
    if (rc != FR_OK) {
        testPassed = false;
    }
    print_result_code(rc);
    xprintf("%d bytes written\r\n", s2);
    
    comm_puts("================== Close 'WRITEME.TXT' file:\r\n");
    rc = f_close(&File1);
    if (rc != FR_OK) {
        testPassed = false;
    }
    print_result_code(rc);

    comm_puts("================== Unmount Logical Drive:\r\n");
    rc = f_mount(0, NULL);
    if (rc != FR_OK) {
        testPassed = false;
    }
    print_result_code(rc);

    if (testPassed) {
        lcd2004_write_string_4d("TEST OK");
        comm_puts("================== SD-CARD TEST OK\r\n");
    }
    else {
        lcd2004_write_string_4d("TEST FAILED");
        comm_puts("================== SD-CARD TEST FAILED\r\n");
    }

    return 0;
}

