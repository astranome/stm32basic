/*----------------------------------------------------------------------------/
/ Based on Arduino BASIC
/ https://github.com/robinhedwards/ArduinoBASIC
/
/ host.h module is a part of Stm32Basic for stm32 systems.
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

#define SCREEN_WIDTH                            20
#define SCREEN_HEIGHT                           4

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
