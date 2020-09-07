/*
utility.c file is a part of stm32Basic project.

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
#include "../include/utility.h"

static volatile uint32_t systick_cnt;

#ifdef SERIAL_TRACES_ENABLED
const char gimmick[] =
"         __           ________   __               _     \r\n"\
"   _____/ /_____ ___ |__  /__ \\ / /__ ____ ______(_)____\r\n"\
"  / ___/ __/ __ `__ \\ /_ <__/ // /__ \\ __ `/ ___/ / ___/\r\n"\
" (__  ) /_/ / / / / /__/ / __// /_/ / /_/ (__  ) / /__  \r\n"\
"/____/\\__/_/ /_/ /_/____/____/_____/\\__,_/____/_/\\___/\r\n";
#endif

const char globalVer[] = "v.0.8.2";
const char newL[] = "\r\n";

void clock_setup(void) {
    rcc_clock_setup_in_hse_8mhz_out_72mhz(); /* 72 MHz */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

#ifdef SERIAL_TRACES_ENABLED
    rcc_periph_clock_enable(RCC_USART1);
#endif

    rcc_periph_clock_enable(RCC_I2C1);
    rcc_periph_clock_enable(RCC_SPI1);
    rcc_periph_clock_enable(RCC_AFIO);
    systick_setup();
}

void ext_interrupt_setup(void) {
    /* Enable EXTI0 interrupt */
    nvic_enable_irq(NVIC_EXTI0_IRQ);

    /* Set PS2 clock pin (in GPIO port A) to 'input open-drain' */
    gpio_set_mode(
        PS2_CLOCK_PORT,
        GPIO_MODE_INPUT,
        GPIO_CNF_INPUT_FLOAT,
        PS2_CLOCK_PIN);

    /* Configure the EXTI subsystem */
    exti_select_source(EXTI0, GPIOA);
    exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);
    exti_enable_request(EXTI0);
}

void misc_gpio_setup(void) {
    /* Set PS2 data pin (in GPIO port B) to 'input open-drain' */
    gpio_set_mode(
        PS2_DATA_PORT,
        GPIO_MODE_INPUT,
        GPIO_CNF_INPUT_FLOAT,
        PS2_DATA_PIN);
}

#ifdef SERIAL_TRACES_ENABLED
void usart_setup(void) {
    /* Setup GPIO pin GPIO_USART1_TX/GPIO9 on GPIO port A for transmit */
    gpio_set_mode(
        GPIOA,
        GPIO_MODE_OUTPUT_50_MHZ,
        GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
        GPIO_USART1_TX);

    /* Setup UART parameters */
    usart_set_baudrate(USART1, UART_SPEED);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

    /* Finally enable the USART */
    usart_enable(USART1);
}
#endif

void i2c_setup(void) {
    /* Set alternate functions for the SCL and SDA pins of I2C2 */
    gpio_set_mode(
        I2C_PORT,
        GPIO_MODE_OUTPUT_50_MHZ,
        GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
        I2C_SCL | I2C_SDA);

    /* Disable the I2C before changing any configuration */
    i2c_peripheral_disable(I2C1);

    /* APB1 is running at 36MHz */
    i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_36MHZ);

    /* 400KHz - I2C Fast Mode */
    i2c_set_fast_mode(I2C1);

    /* 
    fclock for I2C is 36MHz APB2 -> cycle time 28ns, low time at 400kHz
    incl trise -> Thigh = 1600ns; CCR = tlow/tcycle = 0x1C,9;
    Datasheet suggests 0x1e.
    */
    i2c_set_ccr(I2C1, 0x1e);

    /* 
    fclock for I2C is 36MHz -> cycle time 28ns, rise time for
    400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
    Incremented by 1 -> 11.
    */
    i2c_set_trise(I2C1, 0x0b);

    /*
    This is our slave address - needed only if we want to receive from
    other masters.
    i2c_set_own_7bit_slave_address(I2C2, 0x32);
    */

    /* If everything is configured -> enable the peripheral */
    i2c_peripheral_enable(I2C1);
}

void i2c_write_byte(uint8_t address, uint8_t data) {
    /* Send START condition */
    i2c_send_start(I2C1);

    /* Waiting for START is send and switched to master mode */
    while (!((I2C_SR1(I2C1) & I2C_SR1_SB)
        & (I2C_SR2(I2C1) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

    /* Send destination address */
    i2c_send_7bit_address(I2C1, address, I2C_WRITE);

    /* Waiting for address is transferred */
    while (!(I2C_SR1(I2C1) & I2C_SR1_ADDR));

    /* Cleaning ADDR condition sequence */
    I2C_SR2(I2C1);

    /* Sending the data */
    i2c_send_data(I2C1, data);
    while (!(I2C_SR1(I2C1) & I2C_SR1_BTF));

    /* After the last byte we have to wait for TxE too */
    while (!(I2C_SR1(I2C1) & (I2C_SR1_BTF | I2C_SR1_TxE)));

    /* Send STOP condition */
    i2c_send_stop(I2C1);
}

void systick_setup(void) {
    systick_cnt = 0;

    /* 72MHz / 8 => 9000000 counts per second */
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

    /* 
    9000000/900 = 10000 overflows per second - every 0.1 ms one interrupt
    SysTick interrupt every N clock pulses: set reload to N-1
    */
    systick_set_reload(899);
    systick_interrupt_enable();

    /* Start counting */
    systick_counter_enable();
}

void sys_tick_handler(void) {
    systick_cnt++;
#ifdef SD_CARD_IN_USE
    if((systick_cnt % 100) == 0)
    {
        disk_timerproc(); /* 0.1ms * 100 = 10 ms */
    }
#endif
}

uint32_t get_sys_tick(void) {
    return systick_cnt;
}

void delay_us100(uint32_t us100) {
    uint32_t systick_trhld = systick_cnt + us100;
    while(systick_cnt < systick_trhld);
}
