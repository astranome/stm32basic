/*
utility.h file is a part of stm32Basic project.

Copyright (c) 2020 vitasam

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
#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/exti.h>
#include "../include/term_io.h"
#ifdef SERIAL_TRACES_ENABLED
#include <libopencm3/stm32/usart.h>
#endif

/* Arduino legacy code workaround */
#define pgm_read_byte(x)                        (*(x))
#define pgm_read_word(x)                        (*(x))
#define pgm_read_float(x)                       (*(x))
#define pgm_read_byte_near(x)                   (*(x))
#define PROGMEM

#define PS2_CLOCK_PORT                          GPIOA
#define PS2_CLOCK_PIN                           GPIO0
#define PS2_DATA_PORT                           GPIOB
#define PS2_DATA_PIN                            GPIO8

#define SPI_SD_PORT                             GPIOA
#define SPI_SD_CS_PIN                           GPIO4
#define SPI_SD_SCK_PIN                          GPIO5
#define SPI_SD_MISO_PIN                         GPIO6
#define SPI_SD_MOSI_PIN                         GPIO7
#define SPI_SD                                  SPI1

#define I2C_PORT                                GPIOB
#define I2C_SCL                                 GPIO6
#define I2C_SDA                                 GPIO7

#define BOARD_LED_PORT                          GPIOC
#define BOARD_LED_GPIO                          GPIO13

#define SPI_FAST                                SPI_CR1_BR_FPCLK_DIV_4 /* 72MHz/4 */
#define SPI_SLOW                                SPI_CR1_BR_FPCLK_DIV_256 /* 72MHz/256 */
#define SOCKET_WP_CONNECTED                     0 /* Write-protect socket-switch */
#define SOCKET_CP_CONNECTED                     0 /* Card-present socket-switch */
#define CARD_SUPPLY_SWITCHABLE                  0
#define RCC_GPIO                                RCC_APB2ENR
#define RCC_GPIO_PORT_CS                        RCC_APB2ENR_IOPAEN
#define RCC_SPI                                 RCC_APB2ENR
#define RCC_SPI_SD                              RCC_APB2ENR_SPI1EN
#define GPIO_PORT_SPI_SD                        GPIOA
#define USARTx                                  USART1
#define UART_SPEED                              115200
#define SEC_1                                   10000       /* 1 sec */
#define SEC_2                                   20000       /* 2 sec */
#define MS_250                                  2500        /* 250 msec */

#ifdef SERIAL_TRACES_ENABLED
#define DEBUG_SERIAL_PRINT(...) do{xprintf(__VA_ARGS__ ); xprintf("\t\n");} while(false)
#else
#define DEBUG_SERIAL_PRINT(...)
#endif

/* 
 Display capability structure, any display driver should return via API call.
 If some of parameter is not applicable, the parameter should be -1.
 */
#define DISPLAY_NAME_LENGTN  16
typedef struct {
    int displayWidthPixels; /* Display wifth, pixels */
    int displayHeightPixels; /* Display height, pixels */
    int displayWidthSymbols; /* Display wifth, symbols */
    int displayHeightSymbols; /* Display height, symbols */
    int displayHasBacklight; /* 1: display has a backligh; 0: does not have */
    int displayIsMonochome; /* 1: display is monochrome; 0: display supports colors */
    char displayName[DISPLAY_NAME_LENGTN]; /* Display name */
} DisplayCapability;

extern const char gimmick[];
extern const char globalVer[];
extern const char newL[];

void clock_setup(void);
void systick_setup(void);
void ext_interrupt_setup(void);
void misc_gpio_setup(void);
uint32_t get_sys_tick(void);
void delay_us100(uint32_t us100);

#ifdef SERIAL_TRACES_ENABLED
void usart_setup(void);
#endif

void i2c_setup(void);
void i2c_write_byte(uint8_t address, uint8_t data);
void disk_timerproc(void);      /* This function must be called in period of 10ms */
#endif
