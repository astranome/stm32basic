/*----------------------------------------------------------------------------/
/ Based on Martin Thomas 4/2009 code
/
/ fattime.c module is a part of Stm32Basic for stm32 systems.
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
#include "../chan_fatfs/src/integer.h"
#include "../include/rtc.h"
#include "../chan_fatfs/src/ff.h"

DWORD get_fattime (void)
{
    DWORD res;
    RTC_t rtc;

    rtc_gettime( &rtc );

    res =  (((DWORD)rtc.year - 1980) << 25)
            | ((DWORD)rtc.month << 21)
            | ((DWORD)rtc.mday << 16)
            | (WORD)(rtc.hour << 11)
            | (WORD)(rtc.min << 5)
            | (WORD)(rtc.sec >> 1);

    return res;
}

