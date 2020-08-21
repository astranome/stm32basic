/*----------------------------------------------------------------------------/
/ utility.h module is a part of Stm32Basic for stm32 systems.
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
#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>

#ifdef SERIAL_TRACES_ENABLED
#include <libopencm3/stm32/usart.h>
#endif

#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/exti.h>
#include "../include/term_io.h"

/* Port A */
#define PS2_CLOCK_PIN                           GPIO0
#define GPIOSPI_SD_CS                           GPIO4
#define GPIOSPI_SD_SCK                          GPIO5
#define GPIOSPI_SD_MISO                         GPIO6
#define GPIOSPI_SD_MOSI                         GPIO7

/* Port B */
#define I2C_SCL                                 GPIO6
#define I2C_SDA                                 GPIO7

#define LCD_E                                   GPIO10
#define LCD_RS                                  GPIO11
#define LCD_D4                                  GPIO12
#define LCD_D5                                  GPIO13
#define LCD_D6                                  GPIO14
#define LCD_D7                                  GPIO15
#define LCD_BACKLIGHT                           GPIO1
#define PS2_DATA_PIN                            GPIO8

/* Port C */
#define LED_GPIO                                GPIO13

/* Misc. */
#define SPI_FAST                                SPI_CR1_BR_FPCLK_DIV_4        /* 72MHz/4 */
#define SPI_SLOW                                SPI_CR1_BR_FPCLK_DIV_256      /* 72MHz/256 */
#define SPI_SD                                  SPI1
#define SOCKET_WP_CONNECTED                     0 /* Write-protect socket-switch */
#define SOCKET_CP_CONNECTED                     0 /* Card-present socket-switch */
#define CARD_SUPPLY_SWITCHABLE                  0
#define GPIO_PORT_CS                            GPIOA
#define RCC_GPIO                                RCC_APB2ENR
#define RCC_GPIO_PORT_CS                        RCC_APB2ENR_IOPAEN
#define RCC_SPI                                 RCC_APB2ENR
#define RCC_SPI_SD                              RCC_APB2ENR_SPI1EN
#define GPIO_PORT_SPI_SD                        GPIOA
#define USARTx                                  USART1
#define UART_SPEED                              115200
#define LCD_SCREEN_WIDTH                        20
#define LCD_SCREEN_HEIGHT                       4
#define SEC_1                                   10000       /* 1 sec */
#define SEC_2                                   20000       /* 2 sec */
#define MS_250                                  2500        /* 250 msec */

#ifdef SERIAL_TRACES_ENABLED
#define DEBUG_SERIAL_PRINT(...) do{xprintf(__VA_ARGS__ ); xprintf("\t\n");} while(false)
#else
#define DEBUG_SERIAL_PRINT(...) do { } while (false)
#endif

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
void lcd_setup(void);
void disk_timerproc(void);      /* This function must be called in period of 10ms */
#endif
