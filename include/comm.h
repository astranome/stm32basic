/*
comm.h file is a part of stm32Basic project.

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
#ifndef COMM_H_
#define COMM_H_

void comm_init(void);
int  comm_test(void);
void comm_put(char);
void comm_puts(const char*);
char comm_get(void);

#endif

