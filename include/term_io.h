/*----------------------------------------------------------------------------/
/ Based on 'Demo for ChaN's FAT-module on STMicroelectronics STM32
/ microcontroller' by Martin Thomas, Kaiserslautern,
/ Germany <mthomas(at)rhrk.uni-kl.de>
/
/ term_io.h module is a part of Stm32Basic for stm32 systems.
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
#ifndef TERM_IO_H_
#define TERM_IO_H_

#include "../chan_fatfs/src/integer.h"
#include "comm.h"

#define xgetc() (char)comm_get()
#define xavail() comm_test()

int xatoi (char**, long*);
void xputc (char);
void xputs (const char*);
void xitoa (long, int, int);
void xprintf (const char*, ...) __attribute__ ((format (__printf__, 1, 2)));
void put_dump (const BYTE*, DWORD ofs, int cnt);
void get_line (char*, int len);
int get_line_r (char*, int len, int *idx);
#endif

