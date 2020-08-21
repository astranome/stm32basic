/*----------------------------------------------------------------------------/
/ Based on
/ https://github.com/PaulStoffregen/PS2Keyboard
/
/ ps2kbd.c module is a part of Stm32Basic for stm32 systems.
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
#include <stdio.h>
#include <string.h>
#include "../include/ps2kbd.h"
#include "../include/utility.h"
#include "../include/host.h"

#define BUFFER_SIZE                             45

static volatile uint8_t buffer[BUFFER_SIZE];
static volatile uint8_t head, tail;
static uint8_t CharBuffer = 0;
static uint8_t UTF8next = 0;
static const PS2Keymap_t *keymap = NULL;

/*
 * http://www.quadibloc.com/comp/scan.htm
 * http://www.computer-engineering.org/ps2keyboard/scancodes2.html
 *
 * These arrays provide a simple key map, to turn scan codes into ISO8859
 * output. If a non-US keyboard is used, these may need to be modified
 * for the desired output.
 */

#ifdef KEYBOARD_US
const PS2Keymap_t PS2Keymap_US =
{
    /* Without shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '`', 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
    0, 0, 'z', 's', 'a', 'w', '2', 0,
    0, 'c', 'x', 'd', 'e', '4', '3', 0,
    0, ' ', 'v', 'f', 't', 'r', '5', 0,
    0, 'n', 'b', 'h', 'g', 'y', '6', 0,
    0, 0, 'm', 'j', 'u', '7', '8', 0,
    0, ',', 'k', 'i', 'o', '0', '9', 0,
    0, '.', '/', 'l', ';', 'p', '-', 0,
    0, 0, '\'', 0, '[', '=', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '\\', 0, 0,
    0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },

    /* With shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '~', 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
    0, 0, 'Z', 'S', 'A', 'W', '@', 0,
    0, 'C', 'X', 'D', 'E', '$', '#', 0,
    0, ' ', 'V', 'F', 'T', 'R', '%', 0,
    0, 'N', 'B', 'H', 'G', 'Y', '^', 0,
    0, 0, 'M', 'J', 'U', '&', '*', 0,
    0, '<', 'K', 'I', 'O', ')', '(', 0,
    0, '>', '?', 'L', ':', 'P', '_', 0,
    0, 0, '"', 0, '{', '+', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '}', 0, '|', 0, 0,
    0, 0, 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },
    0
};
#endif

#ifdef KEYBOARD_GERMAN
const PS2Keymap_t PS2Keymap_German =
{
    /* Without shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '^', 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
    0, 0, 'y', 's', 'a', 'w', '2', 0,
    0, 'c', 'x', 'd', 'e', '4', '3', 0,
    0, ' ', 'v', 'f', 't', 'r', '5', 0,
    0, 'n', 'b', 'h', 'g', 'z', '6', 0,
    0, 0, 'm', 'j', 'u', '7', '8', 0,
    0, ',', 'k', 'i', 'o', '0', '9', 0,
    0, '.', '-', 'l', PS2_o_DIAERESIS, 'p', PS2_SHARP_S, 0,
    0, 0, PS2_a_DIAERESIS, 0, PS2_u_DIAERESIS, '\'', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '+', 0, '#', 0, 0,
    0, '<', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },

    /* With shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, PS2_DEGREE_SIGN, 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
    0, 0, 'Y', 'S', 'A', 'W', '"', 0,
    0, 'C', 'X', 'D', 'E', '$', PS2_SECTION_SIGN, 0,
    0, ' ', 'V', 'F', 'T', 'R', '%', 0,
    0, 'N', 'B', 'H', 'G', 'Z', '&', 0,
    0, 0, 'M', 'J', 'U', '/', '(', 0,
    0, ';', 'K', 'I', 'O', '=', ')', 0,
    0, ':', '_', 'L', PS2_O_DIAERESIS, 'P', '?', 0,
    0, 0, PS2_A_DIAERESIS, 0, PS2_U_DIAERESIS, '`', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '*', 0, '\'', 0, 0,
    0, '>', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },
    1,

    /* With altgr */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0, 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, '@', 0, 0,
    0, 0, 0, 0, 0, 0, PS2_SUPERSCRIPT_TWO, 0,
    0, 0, 0, 0, PS2_CURRENCY_SIGN, 0, PS2_SUPERSCRIPT_THREE, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, PS2_MICRO_SIGN, 0, 0, '{', '[', 0,
    0, 0, 0, 0, 0, '}', ']', 0,
    0, 0, 0, 0, 0, 0, '\\', 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '~', 0, '#', 0, 0,
    0, '|', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 }
};
#endif

#ifdef KEYBOARD_FRENCH
const PS2Keymap_t PS2Keymap_French =
{
    /* Without shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, PS2_SUPERSCRIPT_TWO, 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'a', '&', 0,
    0, 0, 'w', 's', 'q', 'z', PS2_e_ACUTE, 0,
    0, 'c', 'x', 'd', 'e', '\'', '"', 0,
    0, ' ', 'v', 'f', 't', 'r', '(', 0,
    0, 'n', 'b', 'h', 'g', 'y', '-', 0,
    0, 0, ',', 'j', 'u', PS2_e_GRAVE, '_', 0,
    0, ';', 'k', 'i', 'o', PS2_a_GRAVE, PS2_c_CEDILLA, 0,
    0, ':', '!', 'l', 'm', 'p', ')', 0,
    0, 0, PS2_u_GRAVE, 0, '^', '=', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '$', 0, '*', 0, 0,
    0, '<', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },

    /* With shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0, 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'A', '1', 0,
    0, 0, 'W', 'S', 'Q', 'Z', '2', 0,
    0, 'C', 'X', 'D', 'E', '4', '3', 0,
    0, ' ', 'V', 'F', 'T', 'R', '5', 0,
    0, 'N', 'B', 'H', 'G', 'Y', '6', 0,
    0, 0, '?', 'J', 'U', '7', '8', 0,
    0, '.', 'K', 'I', 'O', '0', '9', 0,
    0, '/', PS2_SECTION_SIGN, 'L', 'M', 'P', PS2_DEGREE_SIGN, 0,
    0, 0, '%', 0, PS2_DIAERESIS, '+', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, PS2_POUND_SIGN, 0, PS2_MICRO_SIGN, 0, 0,
    0, '>', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },
    1,

    /* With altgr */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 0, 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, '@', 0, 0,
    0, 0, 0, 0, 0, 0, '~', 0,
    0, 0, 0, 0, 0 /*PS2_EURO_SIGN*/, '{', '#', 0,
    0, 0, 0, 0, 0, 0, '[', 0,
    0, 0, 0, 0, 0, 0, '|', 0,
    0, 0, 0, 0, 0, '`', '\\', 0,
    0, 0, 0, 0, 0, '@', '^', 0,
    0, 0, 0, 0, 0, 0, ']', 0,
    0, 0, 0, 0, 0, 0, '}', 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, PS2_CURRENCY_SIGN, 0, '#', 0, 0,
    0, '|', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 }
};
#endif

#ifdef KEYBOARD_SPANISH
const PS2Keymap_t PS2Keymap_Spanish =
{
    /* Without shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
     0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, PS2_MASCULINE_ORDINAL, 0,
     0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
     0, 0, 'z', 's', 'a', 'w', '2', 0,
     0, 'c', 'x', 'd', 'e', '4', '3', 0,
     0, ' ', 'v', 'f', 't', 'r', '5', 0,
     0, 'n', 'b', 'h', 'g', 'y', '6', 0,
     0, 0, 'm', 'j', 'u', '7', '8', 0,
     0, ',', 'k', 'i', 'o', '0', '9', 0,
     0, '.', '-', 'l', 'n', 'p', '\'', 0,
     0, 0, PS2_ACUTE_ACCENT, 0, '`', PS2_INVERTED_EXCLAMATION, 0, 0,
     0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '+', 0, PS2_c_CEDILLA, 0, 0,
     0, '<', 0, 0, 0, 0, PS2_BACKSPACE, 0,
     0, '1', 0, '4', '7', 0, 0, 0,
     '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
     PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
     0, 0, 0, PS2_F7 },

    /* With shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
     0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, PS2_FEMININE_ORDINAL, 0,
     0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
     0, 0, 'Z', 'S', 'A', 'W', '\"', 0,
     0, 'C', 'X', 'D', 'E', '$', PS2_MIDDLE_DOT, 0,
     0, ' ', 'V', 'F', 'T', 'R', '%', 0,
     0, 'N', 'B', 'H', 'G', 'Y', '&', 0,
     0, 0, 'M', 'J', 'U', '/', '(', 0,
     0, ';', 'K', 'I', 'O', '=', ')', 0,
     0, ':', '_', 'L', 'N', 'P', '?', 0,
     0, 0, PS2_DIAERESIS, 0, '^', PS2_INVERTED_QUESTION_MARK, 0, 0,
     0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '*', 0, PS2_C_CEDILLA, 0, 0,
     0, '>', 0, 0, 0, 0, PS2_BACKSPACE, 0,
     0, '1', 0, '4', '7', 0, 0, 0,
     '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
     PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
     0, 0, 0, PS2_F7 },
     1,

    /* With altgr */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
     0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '\\', 0,
     0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '|', 0,
     0, 0, 'z', 's', 'a', 'w', '@', 0,
     0, 'c', 'x', 'd', PS2_EURO_SIGN, '~', '#', 0,
     0, ' ', 'v', 'f', 't', 'r', '5', 0,
     0, 'n', 'b', 'h', 'g', 'y', PS2_NOT_SIGN, 0,
     0, 0, 'm', 'j', 'u', '7', '8', 0,
     0, ',', 'k', 'i', 'o', '0', '9', 0,
     0, '.', '-', 'l', 'n', 'p', '\'', 0,
     0, 0, '{', 0, '[', PS2_INVERTED_EXCLAMATION, 0, 0,
     0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '}', 0, 0,
     0, '|', 0, 0, 0, 0, PS2_BACKSPACE, 0,
     0, '1', 0, '4', '7', 0, 0, 0,
     '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
     PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
     0, 0, 0, PS2_F7 }
};
#endif

#ifdef KEYBOARD_ITALIAN
const PS2Keymap_t PS2Keymap_Italian =
{
    /* Without shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
     0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '\\', 0,
     0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
     0, 0, 'z', 's', 'a', 'w', '2', 0,
     0, 'c', 'x', 'd', 'e', '4', '3', 0,
     0, ' ', 'v', 'f', 't', 'r', '5', 0,
     0, 'n', 'b', 'h', 'g', 'y', '6', 0,
     0, 0, 'm', 'j', 'u', '7', '8', 0,
     0, ',', 'k', 'i', 'o', '0', '9', 0,
     0, '.', '-', 'l', PS2_o_GRAVE, 'p', '\'', 0,
     0, 0, PS2_a_GRAVE, 0, PS2_e_GRAVE, PS2_i_GRAVE, 0, 0,
     0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '+', 0, PS2_u_GRAVE, 0, 0,
     0, '<', 0, 0, 0, 0, PS2_BACKSPACE, 0,
     0, '1', 0, '4', '7', 0, 0, 0,
     '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
     PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
     0, 0, 0, PS2_F7 },

    /* With shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
     0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '|', 0,
     0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
     0, 0, 'Z', 'S', 'A', 'W', '\"', 0,
     0, 'C', 'X', 'D', 'E', '$', PS2_POUND_SIGN, 0,
     0, ' ', 'V', 'F', 'T', 'R', '%', 0,
     0, 'N', 'B', 'H', 'G', 'Y', '&', 0,
     0, 0, 'M', 'J', 'U', '/', '(', 0,
     0, ';', 'K', 'I', 'O', '=', ')', 0,
     0, ':', '_', 'L', PS2_c_CEDILLA, 0, '?', 0,
     0, 0, PS2_DEGREE_SIGN, 0, PS2_e_ACUTE, '^', 0, 0,
     0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '*', 0, PS2_SECTION_SIGN, 0, 0,
     0, '>', 0, 0, 0, 0, PS2_BACKSPACE, 0,
     0, '1', 0, '4', '7', 0, 0, 0,
     '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
     PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
     0, 0, 0, PS2_F7 },
     1,

    /* With altgr */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
     0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, PS2_NOT_SIGN, 0,
     0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', PS2_SUPERSCRIPT_ONE, 0,
     0, 0, 'z', 's', 'a', 'w', PS2_SUPERSCRIPT_TWO, 0,
     0, 'c', 'x', 'd', PS2_EURO_SIGN, PS2_FRACTION_ONE_QUARTER, PS2_SUPERSCRIPT_THREE, 0,
     0, ' ', 'v', 'f', 't', 'r', PS2_FRACTION_ONE_HALF, 0,
     0, 'n', 'b', 'h', 'g', 'y', PS2_NOT_SIGN, 0,
     0, 0, 'm', 'j', 'u', '{', '[', 0,
     0, ',', 'k', 'i', 'o', '}', ']', 0,
     0, '.', '-', 'l', '@', 'p', '\'', 0,
     0, 0, '#', 0, '[', '~', 0, 0,
     0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, 0, 0, 0,
     0, PS2_LEFT_DOUBLE_ANGLE_QUOTE, 0, 0, 0, 0, PS2_BACKSPACE, 0,
     0, '1', 0, '4', '7', 0, 0, 0,
     '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
     PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
     0, 0, 0, PS2_F7 }
};
#endif

#ifdef KEYBOARD_UK
const PS2Keymap_t PS2Keymap_UK =
{
    /* https://github.com/PaulStoffregen/PS2Keyboard/issues/15
       without shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, '`', 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'q', '1', 0,
    0, 0, 'z', 's', 'a', 'w', '2', 0,
    0, 'c', 'x', 'd', 'e', '4', '3', 0,
    0, ' ', 'v', 'f', 't', 'r', '5', 0,
    0, 'n', 'b', 'h', 'g', 'y', '6', 0,
    0, 0, 'm', 'j', 'u', '7', '8', 0,
    0, ',', 'k', 'i', 'o', '0', '9', 0,
    0, '.', '/', 'l', ';', 'p', '-', 0,
    0, 0, '\'', 0, '[', '=', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, ']', 0, '#', 0, 0,
    0, '\\', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },

    /* With shift */
    {0, PS2_F9, 0, PS2_F5, PS2_F3, PS2_F1, PS2_F2, PS2_F12,
    0, PS2_F10, PS2_F8, PS2_F6, PS2_F4, PS2_TAB, 172 /* ¬ */, 0,
    0, 0 /*Lalt*/, 0 /*Lshift*/, 0, 0 /*Lctrl*/, 'Q', '!', 0,
    0, 0, 'Z', 'S', 'A', 'W', '"', 0,
    0, 'C', 'X', 'D', 'E', '$', 163 /* £ */, 0,
    0, ' ', 'V', 'F', 'T', 'R', '%', 0,
    0, 'N', 'B', 'H', 'G', 'Y', '^', 0,
    0, 0, 'M', 'J', 'U', '&', '*', 0,
    0, '<', 'K', 'I', 'O', ')', '(', 0,
    0, '>', '?', 'L', ':', 'P', '_', 0,
    0, 0, '@', 0, '{', '+', 0, 0,
    0 /*CapsLock*/, 0 /*Rshift*/, PS2_ENTER /*Enter*/, '}', 0, '~', 0, 0,
    0, '|', 0, 0, 0, 0, PS2_BACKSPACE, 0,
    0, '1', 0, '4', '7', 0, 0, 0,
    '0', '.', '2', '5', '6', '8', PS2_ESC, 0 /*NumLock*/,
    PS2_F11, '+', '3', '-', '*', '9', PS2_SCROLL, 0,
    0, 0, 0, PS2_F7 },
    0
};
#endif

#define BREAK                           0x01
#define MODIFIER                        0x02
#define SHIFT_L                         0x04
#define SHIFT_R                         0x08
#define ALTGR                           0x10


/* The ISR for the external interrupt */
void exti0_isr(void)
{
    static uint8_t bitcount = 0;
    static uint8_t incoming = 0;
    static uint32_t prev_tick = 0;
    uint32_t now_tick;
    uint8_t n, val;

    exti_reset_request(EXTI0);

    val = (gpio_get(GPIOB, PS2_DATA_PIN) >> 8) & 0x01;
    now_tick = get_sys_tick();
    if (now_tick - prev_tick > MS_250)
    {
        bitcount = 0;
        incoming = 0;
    }

    prev_tick = now_tick;
    n = bitcount - 1;
    if (n <= 7)
    {
        incoming |= (val << n);
    }

    bitcount++;

    if (bitcount == 11)
    {
        uint8_t i = head + 1;
        if (i >= BUFFER_SIZE) i = 0;
        if (i != tail)
        {
            buffer[i] = incoming;
            head = i;
        }

        bitcount = 0;
        incoming = 0;
    }

    exti_enable_request(EXTI0);
}


static inline uint8_t get_scan_code(void)
{
    uint8_t c, i;

    i = tail;
    if (i == head)
    {
        return 0;
    }

    i++;
    if (i >= BUFFER_SIZE)
    {
        i = 0;
    }

    c = buffer[i];
    tail = i;
    return c;
}


static char get_iso8859_code(void)
{
    static uint8_t state=0;
    uint8_t s;
    char c;

    while (1)
    {
        s = get_scan_code();
        if (!s)
        {
            return 0;
        }

        DEBUG_SERIAL_PRINT("Scan code:%d", s);

        if (s == 0xF0)
        {
            state |= BREAK;
        }
        else if (s == 0xE0)
        {
            state |= MODIFIER;
        }
        else
        {
            if (state & BREAK)
            {
                if (s == 0x12)
                {
                    state &= ~SHIFT_L;
                }
                else if (s == 0x59)
                {
                    state &= ~SHIFT_R;
                }
                else if (s == 0x11 && (state & MODIFIER))
                {
                    state &= ~ALTGR;
                }

                /* CTRL, ALT & WIN keys could be added
                   but is that really worth the overhead? */
                state &= ~(BREAK | MODIFIER);
                continue;
            }

            if (s == 0x12)
            {
                state |= SHIFT_L;
                continue;
            }
            else if (s == 0x59)
            {
                state |= SHIFT_R;
                continue;
            }
            else if (s == 0x11 && (state & MODIFIER))
            {
                state |= ALTGR;
            }

            c = 0;
            if (state & MODIFIER)
            {
                switch (s)
                {
                    case 0x70: c = PS2_INSERT;      break;
                    case 0x6C: c = PS2_HOME;        break;
                    case 0x7D: c = PS2_PAGEUP;      break;
                    case 0x71: c = PS2_DELETE;      break;
                    case 0x69: c = PS2_END;         break;
                    case 0x7A: c = PS2_PAGEDOWN;    break;
                    case 0x75: c = PS2_UPARROW;     break;
                    case 0x6B: c = PS2_LEFTARROW;   break;
                    case 0x72: c = PS2_DOWNARROW;   break;
                    case 0x74: c = PS2_RIGHTARROW;  break;
                    case 0x4A: c = '/';             break;
                    case 0x5A: c = PS2_ENTER;       break;

                    default: break;
                }
            }
#ifndef KEYBOARD_US
            else if ((state & ALTGR) && keymap->uses_altgr)
            {
                if (s < PS2_KEYMAP_SIZE)
                {
                    c = pgm_read_byte(keymap->altgr + s);
                }
            }
#endif
            else if (state & (SHIFT_L | SHIFT_R))
            {
                if (s < PS2_KEYMAP_SIZE)
                {
                    c = pgm_read_byte(keymap->shift + s);
                }
            }
            else
            {
                if (s < PS2_KEYMAP_SIZE)
                {
                    c = pgm_read_byte(keymap->noshift + s);
                }
            }

            state &= ~(BREAK | MODIFIER);
            if (c)
            {
                return c;
            }
        }
    }
}


bool kbd_available(void)
{
    if (CharBuffer || UTF8next)
    {
        return true;
    }

    CharBuffer = get_iso8859_code();
    if (CharBuffer)
    {
        return true;
    }

    return false;
}


void kbd_clear(void)
{
    CharBuffer = 0;
    UTF8next = 0;
}


uint8_t kbd_readScanCode(void)
{
    return get_scan_code();
}


int kbd_read(void)
{
    uint8_t result;

    result = UTF8next;
    if (result)
    {
        UTF8next = 0;
    }
    else
    {
        result = CharBuffer;
        if (result)
        {
            CharBuffer = 0;
        }
        else
        {
            result = get_iso8859_code();
        }

        if (result >= 128)
        {
            UTF8next = (result & 0x3F) | 0x80;
            result = ((result >> 6) & 0x1F) | 0xC0;
        }
    }

    if (!result)
    {
        return -1;
    }

    return result;
}


int kbd_readUnicode(void)
{
    int result;

    result = CharBuffer;
    if (!result)
    {
        result = get_iso8859_code();
    }

    if (!result)
    {
        return -1;
    }

    UTF8next = 0;
    CharBuffer = 0;
    return result;
}


void kbd_begin(void)
{
#ifdef KEYBOARD_US
    keymap = &PS2Keymap_US;
#endif

#ifdef KEYBOARD_GERMAN
    keymap = &PS2Keymap_German;
#endif

#ifdef KEYBOARD_FRENCH
    keymap = &PS2Keymap_French;
#endif

#ifdef KEYBOARD_SPANISH
    keymap = &PS2Keymap_Spanish;
#endif

#ifdef KEYBOARD_ITALIAN
    keymap = &PS2Keymap_Italian;
#endif

#ifdef KEYBOARD_UK
    keymap = &PS2Keymap_UK;
#endif

    head = 0;
    tail = 0;
}
