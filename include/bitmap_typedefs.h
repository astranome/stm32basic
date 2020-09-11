/*
bitmap_typedefs.h file is a part of stm32Basic project.

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
#ifndef INC_BITMAP_TYPEDEFS_H_
#define INC_BITMAP_TYPEDEFS_H_

#include <stdint.h>
#include <inttypes.h>

typedef struct {
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
    } tImage;

typedef struct {
	const uint16_t *data;
	uint16_t width;
	uint16_t height;
	uint8_t dataSize;
	} tImage16bit;

typedef struct {
    long int code;
    const tImage *image;
    } tChar;

typedef struct {
    int length;
    const tChar *chars;
    } tFont;

#endif
