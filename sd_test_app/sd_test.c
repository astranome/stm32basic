/*----------------------------------------------------------------------------/
/ Based on libopencm3 library
/ https://github.com/libopencm3/libopencm3
/
/ sd_test.c module is a part of Stm32Basic for stm32 systems.
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
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#ifdef SERIAL_TRACES_ENABLED
#include <libopencm3/stm32/usart.h>
#endif

#include "../include/utility.h"
#include "../include/lcd.h"
#include "../include/rtc.h"
#include "../chan_fatfs/src/ff.h"
#include "../chan_fatfs/src/diskio.h"

const char applicationName[] = "SD tester";

#ifdef SERIAL_TRACES_ENABLED
static void put_rc(FRESULT rc)
{
    const char *p;
    static const char str[] =
        "OK\0" "NOT_READY\0" "NO_FILE\0" "FR_NO_PATH\0" "INVALID_NAME\0" "INVALID_DRIVE\0"
        "DENIED\0" "EXIST\0" "RW_ERROR\0" "WRITE_PROTECTED\0" "NOT_ENABLED\0"
        "NO_FILESYSTEM\0" "INVALID_OBJECT\0" "MKFS_ABORTED\0";

    FRESULT i;
    for (p = str, i = 0; i != rc && *p; i++)
    {
        while(*p++);
    }

    xprintf("rc=%u FR_%s\n", (UINT)rc, p);
}
#endif

int main(void)
{
    long p1, p2;
    BYTE Buff[4 * 1024] __attribute__ ((aligned (4))) ;       /* Working buffer */
    UINT s1, s2, cnt;
    WORD res, w1;
    BYTE b1;
    FATFS Fatfs[_VOLUMES];      /* File system object for each logical drive */
    DIR Dir;                    /* Directory object */
    FRESULT rc;
    FILINFO Finfo;
    FATFS *fs;                  /* Pointer to file system object */
    FIL File1;
    DWORD ofs = 0;

    clock_setup();
    usart_setup();
    lcd_setup();

    comm_puts(gimmick);
    comm_puts(globalVer);
    comm_puts(newL);
    comm_puts(applicationName);
    comm_puts(newL);

    lcd_init_4bit_mode();
    lcd_backlight_on();
    lcd_clear();
    lcd_home();
    lcd_write_string_4d(applicationName);
    lcd_set_cursor(0, 1);
    lcd_write_string_4d(globalVer);
    lcd_set_cursor(0, 2);
    lcd_write_string_4d("See results on UART");

    /* Initialize disk */
    comm_puts("================== Init Disk:\r\n");
    res = (WORD)disk_initialize(0);
    xprintf("SD initialization: %s\n", res ? "FAIL" : "OK");

    /* Show disk status */
    Buff[0] = 2;

    if (disk_ioctl(0, CTRL_POWER, Buff) == RES_OK)
    {
        xprintf("Power is %s\n", Buff[1] ? "ON" : "OFF");
    }

    if (disk_ioctl(0, GET_SECTOR_COUNT, &p2) == RES_OK)
    {
        xprintf("Drive size: %lu sectors\n", p2);
    }

    if (disk_ioctl(0, GET_SECTOR_SIZE, &w1) == RES_OK)
    {
        xprintf("Sector size: %u\n", w1);
    }

    if (disk_ioctl(0, GET_BLOCK_SIZE, &p2) == RES_OK)
    {
        xprintf("Erase block size: %lu sectors\n", p2);
    }

    if (disk_ioctl(0, MMC_GET_TYPE, &b1) == RES_OK)
    {
        xprintf("MMC/SDC type: %u\n", b1);
    }

    if (disk_ioctl(0, MMC_GET_CSD, Buff) == RES_OK)
    {
        xputs("CSD:\n"); put_dump(Buff, 0, 16);
    }

    if (disk_ioctl(0, MMC_GET_CID, Buff) == RES_OK)
    {
        xputs("CID:\n"); put_dump(Buff, 0, 16);
    }

    if (disk_ioctl(0, MMC_GET_OCR, Buff) == RES_OK)
    {
        xputs("OCR:\n"); put_dump(Buff, 0, 4);
    }

    if (disk_ioctl(0, MMC_GET_SDSTAT, Buff) == RES_OK)
    {
        xputs("SD Status:\n");
        for (s1 = 0; s1 < 64; s1 += 16)
        {
            put_dump(Buff+s1, s1, 16);
        }
    }

    comm_puts("================== Mount Logical Drive:\r\n");
    rc = f_mount(0, &Fatfs[0]);
    put_rc(rc);

    comm_puts("================== Directory listing:\r\n");
    rc = f_opendir(&Dir, "");
    put_rc(rc);

    p1 = s1 = s2 = 0;
    for(;;)
    {
        rc = f_readdir(&Dir, &Finfo);
        if ((res != FR_OK) || !Finfo.fname[0])
        {
            break;
        }

        if (Finfo.fattrib & AM_DIR)
        {
            s2++;
        }
        else
        {
            s1++;
            p1 += Finfo.fsize;
        }

        xprintf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s",
            (Finfo.fattrib & AM_DIR) ? 'D' : '-',
            (Finfo.fattrib & AM_RDO) ? 'R' : '-',
            (Finfo.fattrib & AM_HID) ? 'H' : '-',
            (Finfo.fattrib & AM_SYS) ? 'S' : '-',
            (Finfo.fattrib & AM_ARC) ? 'A' : '-',
            (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
            (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
            Finfo.fsize, &(Finfo.fname[0]));

        xputc('\n');
    }

    xprintf("%4u File(s),%10lu bytes total\n%4u Dir(s)", s1, p1, s2);
    if (f_getfree("", (DWORD*)&p1, &fs) == FR_OK)
    {
        xprintf(", %10lu bytes free\n", p1 * fs->csize * 512);
    }

    comm_puts("================== Open (FA_READ) 'README.TXT' file:\r\n");
    rc = f_open(&File1, "README.TXT", FA_READ);
    put_rc(rc);

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

        rc = f_read(&File1, Buff, cnt, &cnt);
        
        if (res != FR_OK)
        {
            put_rc(rc);
            break;
        }

        if (!cnt)
        {
            break;
        }

        put_dump(Buff, ofs, cnt);
        ofs += 16;
    }

    comm_puts("================== Close 'README.TXT' file:\r\n");
    rc = f_close(&File1);
    put_rc(rc);

    comm_puts("================== Create (FA_WRITE | FA_CREATE_ALWAYS) 'WRITEME.TXT' file:\r\n");
    rc = f_open(&File1, "WRITEME.TXT", FA_WRITE | FA_CREATE_ALWAYS);
    put_rc(rc);

    comm_puts("================== Write string to 'WRITEME.TXT' file:\r\n");
    memset(Buff, 0, sizeof(Buff));
    char hello[] = {"Hello from stm32Basic!"};
    cnt = sizeof(hello);
	rc = f_write(&File1, hello, cnt, &s2);
    put_rc(rc);
    xprintf("%d bytes written\r\n", s2);
	
    comm_puts("================== Close 'WRITEME.TXT' file:\r\n");
    rc = f_close(&File1);
    put_rc(rc);

    comm_puts("================== Unmount Logical Drive:\r\n");
    rc = f_mount(0, NULL);
    put_rc(rc);

    comm_puts("OK\r\n");
    return 0;
}

