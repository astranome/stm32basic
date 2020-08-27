/*
comm.c file is a part of stm32Basic project.

Copyright (c) 2020 vitasam

Based on Martin Thomas code from 4/2009, 3/2010

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
#include <libopencm3/stm32/usart.h>
#include "../include/utility.h"
#include "../include/comm.h"

int comm_test(void)
{
    return usart_get_flag(USARTx, USART_SR_RXNE);
}

char comm_get(void)
{
    return (char)usart_recv_blocking(USARTx);
}

void comm_put(char d)
{
    usart_send_blocking(USARTx, (uint16_t)d);
}

void comm_puts(const char* s)
{
    char c;
    while ( ( c = *s++) != '\0' )
    {
        comm_put(c);
    }
}

void comm_init (void)
{
    // already done in main.c
}


