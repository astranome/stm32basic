/*
term_io.h file is a part of stm32Basic project.

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

