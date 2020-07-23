/*----------------------------------------------------------------------------/
/ Based on Martin Thomas 4/2009, 3/2010 code
/
/ comm.c module is a part of Stm32Basic for stm32 systems.
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


