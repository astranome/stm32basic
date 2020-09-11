/*
tft320240spi.h file is a part of stm32Basic project.

Copyright (c) 2020 vitasam

Based on ILI9341 driver:
https://github.com/abhra0897/stm32f1_ili9341_parallel

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
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include "../include/utility.h"

#ifndef TFT_320240_SPI_H_
#define TFT_320240_SPI_H_

#define UNUSED(x) (void)(x)

#define ILI_NOP     0x00
#define ILI_SWRESET 0x01
#define ILI_RDDID   0xD3
#define ILI_RDDST   0x09

#define ILI_SLPIN   0x10
#define ILI_SLPOUT  0x11
#define ILI_PTLON   0x12
#define ILI_NORON   0x13

#define ILI_RDMODE  0x0A
#define ILI_RDMADCTL  0x0B
#define ILI_RDPIXFMT  0x0C
#define ILI_RDIMGFMT  0x0D
#define ILI_RDSELFDIAG  0x0F

#define ILI_INVOFF  0x20
#define ILI_INVON   0x21
#define ILI_GAMMASET 0x26
#define ILI_DISPOFF 0x28
#define ILI_DISPON  0x29

#define ILI_CASET   0x2A
#define ILI_PASET   0x2B
#define ILI_RAMWR   0x2C
#define ILI_RAMRD   0x2E

#define ILI_PTLAR   0x30
#define ILI_MADCTL  0x36
#define ILI_PIXFMT  0x3A

#define ILI_FRMCTR1 0xB1
#define ILI_FRMCTR2 0xB2
#define ILI_FRMCTR3 0xB3
#define ILI_INVCTR  0xB4
#define ILI_DFUNCTR 0xB6

#define ILI_PWCTR1  0xC0
#define ILI_PWCTR2  0xC1
#define ILI_PWCTR3  0xC2
#define ILI_PWCTR4  0xC3
#define ILI_PWCTR5  0xC4
#define ILI_VMCTR1  0xC5
#define ILI_VMCTR2  0xC7

#define ILI_RDID1   0xDA
#define ILI_RDID2   0xDB
#define ILI_RDID3   0xDC
#define ILI_RDID4   0xDD

#define ILI_GMCTRP1 0xE0
#define ILI_GMCTRN1 0xE1
/*
#define ILI_PWCTR6  0xFC
*/

#define	R_POS_RGB   11	// Red last bit position for RGB display
#define	G_POS_RGB   5 	// Green last bit position for RGB display
#define	B_POS_RGB   0	// Blue last bit position for RGB display

#define	RGB(R,G,B) \
	(((uint16_t)(R >> 3) << R_POS_RGB) | \
	((uint16_t)(G >> 2) << G_POS_RGB) | \
	((uint16_t)(B >> 3) << B_POS_RGB))

#define	R_POS_BGR   0	// Red last bit position for BGR display
#define	G_POS_BGR   5 	// Green last bit position for BGR display
#define	B_POS_BGR   11	// Blue last bit position for BGR display

#define	BGR(R,G,B) \
	(((uint16_t)(R >> 3) << R_POS_BGR) | \
	((uint16_t)(G >> 2) << G_POS_BGR) | \
	((uint16_t)(B >> 3) << B_POS_BGR))

#define ILI_COLOR_BLACK       ILI_BGR(0,     0,   0)
#define ILI_COLOR_NAVY        ILI_BGR(0,     0, 123)
#define ILI_COLOR_DARKGREEN   ILI_BGR(0,   125,   0)
#define ILI_COLOR_DARKCYAN    ILI_BGR(0,   125, 123)
#define ILI_COLOR_MAROON      ILI_BGR(123,   0,   0)
#define ILI_COLOR_PURPLE      ILI_BGR(123,   0, 123)
#define ILI_COLOR_OLIVE       ILI_BGR(123, 125,   0)
#define ILI_COLOR_LIGHTGREY   ILI_BGR(198, 195, 198)
#define ILI_COLOR_DARKGREY    ILI_BGR(123, 125, 123)
#define ILI_COLOR_BLUE        ILI_BGR(0,     0, 255)
#define ILI_COLOR_GREEN       ILI_BGR(0,   255,   0)
#define ILI_COLOR_CYAN        ILI_BGR(0,   255, 255)
#define ILI_COLOR_RED         ILI_BGR(255,   0,   0)
#define ILI_COLOR_MAGENTA     ILI_BGR(255,   0, 255)
#define ILI_COLOR_YELLOW      ILI_BGR(255, 255,   0)
#define ILI_COLOR_WHITE       ILI_BGR(255, 255, 255)
#define ILI_COLOR_ORANGE      ILI_BGR(255, 165,   0)
#define ILI_COLOR_GREENYELLOW ILI_BGR(173, 255,  41)
#define ILI_COLOR_PINK        ILI_BGR(255, 130, 198)

#define ILI_SPI				SPI1
#define ILI_PORT			GPIOA
#define ILI_RST				GPIO4
#define ILI_DC				GPIO2
#define ILI_SDA				GPIO7	// SPI MOSI Data
#define ILI_SCL				GPIO5	// SPI SCL clock
#define ILI_CS				GPIO6
// TODO #define ILI_LED				GPIO3

#define DC_CMD				GPIO_BRR(ILI_PORT) = ILI_DC
#define DC_DAT				GPIO_BSRR(ILI_PORT) = ILI_DC
#define RST_ACTIVE			GPIO_BRR(ILI_PORT) = ILI_RST
#define RST_IDLE			GPIO_BSRR(ILI_PORT) = ILI_RST
#define CS_ACTIVE			GPIO_BRR(ILI_PORT) = ILI_CS
#define CS_IDLE				GPIO_BSRR(ILI_PORT) = ILI_CS

#define CONFIG_GPIO_CLOCK()	    { \
									rcc_periph_clock_enable(RCC_GPIOB); \
									rcc_periph_clock_enable(RCC_GPIOA); \
									rcc_periph_clock_enable(RCC_AFIO); \
									rcc_periph_clock_enable(RCC_SPI1); \
								}

#define CONFIG_GPIO()			{ \
									/*Configure GPIO pins : PA2 PA3 PA4 PA5 PA7 */ \
									gpio_set_mode(ILI_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, ILI_SCL|ILI_SDA); \
									/* TODO gpio_set_mode(ST_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, ST_DC|ST_BLK|ST_RST|ST_CS); */ \
									gpio_set_mode(ILI_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, ILI_DC|ILI_RST|ILI_CS); \
									/*Configure GPIO pin Output Level */ \
									/* TODO gpio_set(ST_PORT, ST_BLK|ST_RST|ST_DC|ST_CS); */ \
									gpio_set(ILI_PORT, ILI_RST|ILI_DC|ILI_CS); \
									/* Configures PB4 as GPIO */ \
									AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_JNTRST; \
								}

#define CONFIG_SPI()			{ \
									/* Reset SPI, SPI_CR1 register cleared, SPI is disabled */ \
									spi_reset(ILI_SPI); \
									/* Must use SPI_MODE = 2. (CPOL 1, CPHA 0) */\
									/* Read about SPI MODEs: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface*/ \
									spi_init_master(ILI_SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_2, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST); \
									spi_enable_software_slave_management(ILI_SPI); \
									spi_set_nss_high(ILI_SPI); \
									/* Enable SPI1 periph. */ \
									spi_enable(ILI_SPI); \
								}

#define SWAP(a, b)		{uint16_t temp; temp = a; a = b; b = temp;}

// Important: using `while (!(SPI_SR(ST_SPI) & SPI_SR_TXE));` is
// making the transmission unstable. So, replaced it with 
// `while (SPI_SR(ST_SPI) & SPI_SR_BSY);`
#define WRITE_8BIT(d)	do{ \
							SPI_DR(ILI_SPI) = (uint8_t)(d); \
							while (SPI_SR(ILI_SPI) & SPI_SR_BSY); \
						} while(0)


/*
* function prototypes
*/

/*
 * Inline function to send 8 bit command to the display
 * User need not call it
 */
static inline void write_command_8bit(uint8_t cmd)
{
	//CS_ACTIVE;
	DC_CMD;
	WRITE_8BIT(cmd);
}

/*
 * Inline function to send 8 bit data to the display
 * User need not call it
 */
static inline void write_data_8bit(uint8_t dat)
{
	//CS_ACTIVE;
	DC_DAT;
	WRITE_8BIT(dat);
}

/*
 * Inline function to send 16 bit data to the display
 * User need not call it
 */
static inline void write_data_16bit(uint16_t dat)
{
	//CS_ACTIVE;
	DC_DAT;
	WRITE_8BIT((uint8_t)(dat >> 8));
	WRITE_8BIT((uint8_t)dat);
}

/**
 * Set an area for drawing on the display with start row,col and end row,col.
 * User don't need to call it usually, call it only before some functions who don't call it by default.
 * @param x1 start column address.
 * @param y1 start row address.
 * @param x2 end column address.
 * @param y2 end row address.
 */
void ili_set_address_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * Fills `len` number of pixels with `color`.
 * Call ili_set_address_window() before calling this function.
 * @param color 16-bit RGB565 color value
 * @param len 32-bit number of pixels
 */
void ili_fill_color(uint16_t color, uint32_t len);

/**
 * Draw a line from (x0,y0) to (x1,y1) with `width` and `color`.
 * @param x0 start column address.
 * @param y0 start row address.
 * @param x1 end column address.
 * @param y1 end row address.
 * @param width width or thickness of the line
 * @param color 16-bit RGB565 color of the line
 */
void ili_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t width, uint16_t color);

/**
 * Experimental
 * Draw a rectangle without filling it
 * @param x start column address.
 * @param y start row address
 * @param w Width of rectangle
 * @param h height of rectangle
 */
void ili_draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/*
 * Called by ili_draw_line().
 * User need not call it
 */
void plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t width, uint16_t color);

/*
 * Called by ili_draw_line().
 * User need not call it
 */
void plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t width, uint16_t color);

/*
 * Called by ili_draw_line().
 * User need not call it
 */
void ili_draw_fast_h_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t width, uint16_t color);

/*
 * Called by ili_draw_line().
 * User need not call it
 */
void ili_draw_fast_v_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t width, uint16_t color);

/**
 * Rotate the display clockwise or anti-clockwie set by `rotation`
 * @param rotation Type of rotation. Supported values 0, 1, 2, 3
 */
void ili_rotate_display(uint8_t rotation);

/**
 * Initialize the display driver
 */
void ili_init();

/**
 * Fills a rectangular area with `color`.
 * Before filling, performs area bound checking
 * @param x Start col address
 * @param y Start row address
 * @param w Width of rectangle
 * @param h Height of rectangle
 * @param color 16-bit RGB565 color
 */
void ili_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/*
 * Same as `ili_fill_rect()` but does not do bound checking, so it's slightly faster
 */
void ili_fill_rect_fast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

/**
 * Fill the entire display (screen) with `color`
 * @param color 16-bit RGB565 color
 */
void ili_fill_screen(uint16_t color);

/*
 * Render a character glyph on the display. Called by `ili_draw_string_main()`
 * User need NOT call it
 */
void ili_draw_char(uint16_t x, uint16_t y, uint16_t fore_color, uint16_t back_color, const tImage *glyph, uint8_t is_bg);

/**
 * Renders a string by drawing each character glyph from the passed string.
 * Called by `ili_draw_string()` and `ili_draw_string_withbg()`.
 * Text is wrapped automatically if it hits the screen boundary.
 * x_padding and y_padding defines horizontal and vertical distance (in px) between two characters
 * is_bg=1 : Text will habe background color,   is_bg=0 : Text will have transparent background
 * User need NOT call it.
 */
void ili_draw_string_main(uint16_t x, uint16_t y, char *str, uint16_t fore_color, uint16_t back_color, const tFont *font, uint8_t is_bg);

/**
 * Draws a string on the display with `font` and `color` at given position.
 * Background of this string is transparent
 * @param x Start col address
 * @param y Start y address
 * @param str pointer to the string to be drawn
 * @param color 16-bit RGB565 color of the string
 * @param font Pointer to the font of the string
 */
void ili_draw_string(uint16_t x, uint16_t y, char *str, uint16_t color, const tFont *font);

/**
 * Draws a string on the display with `font`, `fore_color`, and `back_color` at given position.
 * The string has background color
 * @param x Start col address
 * @param y Start y address
 * @param str pointer to the string to be drawn
 * @param foe_color 16-bit RGB565 color of the string
 * @param back_color 16-bit RGB565 color of the string's background
 * @param font Pointer to the font of the string
 */
void ili_draw_string_withbg(uint16_t x, uint16_t y, char *str, uint16_t fore_color, uint16_t back_color, const tFont *font);

/**
 * Draw a bitmap image on the display
 * @param x Start col address
 * @param y Start row address
 * @param bitmap Pointer to the image data to be drawn
 */
void ili_draw_bitmap(uint16_t x, uint16_t y, const tImage *bitmap);
//void ili_draw_bitmap_old(uint16_t x, uint16_t y, const tImage16bit *bitmap);

/**
 * Draw a pixel at a given position with `color`
 * @param x Start col address
 * @param y Start row address
 */
void ili_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

#endif
