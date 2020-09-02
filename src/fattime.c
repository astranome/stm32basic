/*
fattime.c file is a part of stm32Basic project.

Copyright (c) 2020 vitasam

Based on Martin Thomas code from 4/2009

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

