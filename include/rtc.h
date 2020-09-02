/*
rtc.h file is a part of stm32Basic project.

Copyright (c) 2020 vitasam

Based on 'Demo for ChaN's FAT-module on STMicroelectronics STM32
microcontroller' by Martin Thomas, Kaiserslautern,
Germany <mthomas(at)rhrk.uni-kl.de>

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
#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t year;  /* 1..4095 */
    uint8_t  month; /* 1..12 */
    uint8_t  mday;  /* 1..31 */
    uint8_t  wday;  /* 0..6, Sunday = 0*/
    uint8_t  hour;  /* 0..23 */
    uint8_t  min;   /* 0..59 */
    uint8_t  sec;   /* 0..59 */
    uint8_t  dst;   /* 0 Winter, !=0 Summer */
} RTC_t;

int rtc_init(void);
bool rtc_gettime (RTC_t*);              /* Get time */
bool rtc_settime (const RTC_t*);        /* Set time */

#endif
