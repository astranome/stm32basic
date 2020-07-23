/*----------------------------------------------------------------------------/
/ Based on 'Demo for ChaN's FAT-module on STMicroelectronics STM32
/ microcontroller' by Martin Thomas, Kaiserslautern,
/ Germany <mthomas(at)rhrk.uni-kl.de>
/
/ rtc.h module is a part of Stm32Basic for stm32 systems.
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
