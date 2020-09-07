/*
host.h file is a part of stm32Basic project.

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
#ifndef _HOST_H_
#define _HOST_H_

#include <stdint.h>
#include <stdbool.h>

// Arduino legacy workaround
#define pgm_read_byte(x)                        (*(x))
#define pgm_read_word(x)                        (*(x))
#define pgm_read_float(x)                       (*(x))
#define pgm_read_byte_near(x)                   (*(x))
#define PROGMEM

#ifdef LCD2004_IN_USE
#define SCREEN_WIDTH                            20
#define SCREEN_HEIGHT                           4
#elif DSO138_PLATFORM
#define SCREEN_WIDTH                            40
#define SCREEN_HEIGHT                           20
#endif

#ifdef FLASHING_CURSOR_IN_USE
#define CURSOR_CHR                              255
#else
#define CURSOR_CHR                              '_'
#endif

#define CHAR_ESC                                27

#ifdef BUZZER_IN_USE
#define BUZZER_PIN    0     /* TODO */
#else
/* Buzzer pin, 0 = disabled/not present */
#define BUZZER_PIN    0
#endif

#define MAGIC_AUTORUN_NUMBER                    0xFC

void host_init(int buzzerPin);
void host_sleep(long ms);
void host_digitalWrite(int pin,int state);
int host_digitalRead(int pin);
int host_analogRead(int pin);
void host_pinMode(int pin, int mode);
void host_click(void);
void host_startupTone(void);
void host_cls(void);
void scroll_buffer(void);
void host_showBuffer(void);
void host_moveCursor(int x, int y);
void host_outputString(char *str);
void host_outputProgMemString(const char *str);
void host_outputChar(char c);
void host_outputFloat(float f);
char *host_floatToStr(float f, char *buf);
int host_outputInt(long val);
void host_newLine(void);
char *host_readLine(void);
char host_getKey(void);
uint8_t host_ESCPressed(void);
void host_outputFreeMem(unsigned int val);
void host_saveProgram(uint8_t autoexec);
void host_loadProgram(void);

#ifdef SD_CARD_IN_USE
bool host_saveSdCard(char *fileName);
bool host_loadSdCard(char *fileName);
#endif
#endif /* _HOST_H_ */
