/*
basic.h file is a part of stm32Basic project.

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

/* ---------------------------------------------------------------------------
 * Basic Interpreter
 * Robin Edwards 2014
 * ---------------------------------------------------------------------------
 * This BASIC is modelled on Sinclair BASIC for the ZX81 and ZX Spectrum. It
 * should be capable of running most of the examples in the manual for both
 * machines, with the exception of anything machine specific (i.e. graphics,
 * sound & system variables).
 *
 * Notes
 *  - All numbers (except line numbers) are floats internally
 *  - Multiple commands are allowed per line, seperated by :
 *  - LET is optional e.g. LET a = 6: b = 7
 *  - MOD provides the modulo operator which was missing from Sinclair BASIC.
 *     Both numbers are first rounded to ints e.g. 5 mod 2 = 1
 *  - CONT can be used to continue from a STOP. It does not continue from any
 *     other error condition.
 *  - Arrays can be any dimension. There is no single char limit to names.
 *  - Like Sinclair BASIC, DIM a(10) and LET a = 5 refer to different 'a's.
 *     One is a simple variable, the other is an array. There is no ambiguity
 *     since the one being referred to is always clear from the context.
 *  - String arrays differ from Sinclair BASIC. DIM a$(5,5) makes an array
 *     of 25 strings, which can be any length. e.g. LET a$(1,1)="long string"
 *  - functions like LEN, require brackets e.g. LEN(a$)
 *  - String manipulation functions are LEFT$, MID$, RIGHT$
 *  - RND is a nonary operator not a function i.e. RND not RND()
 *  - PRINT AT x,y ... is replaced by POSITION x,y : PRINT ...
 *  - LIST takes an optional start and end e.g. LIST 1,100 or LIST 50
 *  - INKEY$ reads the last key pressed from the keyboard, or an empty string
 *     if no key pressed. The (single key) buffer is emptied after the call.
 *     e.g. a$ = INKEY$
 *  - LOAD/SAVE load and save the current program to the EEPROM (1k limit).
 *     SAVE+ will set the auto-run flag, which loads the program automatically
 *     on boot. With a filename e.g. SAVE "test" saves to an external EEPROM.
 *  - DIR/DELETE "filename" - list and remove files from external EEPROM.
 *  - PINMODE <pin>, <mode> - sets the pin mode (0=input, 1=output, 2=pullup)
 *  - PIN <pin>, <state> - sets the pin high (non zero) or low (zero)
 *  - PINREAD(pin) returns pin value, ANALOGRD(pin) for analog pins
 * ---------------------------------------------------------------------------
 */

/*
TODO
EXP etc
DATA, READ, RESTORE
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include "../include/host.h"
#include "../include/basic.h"
#include "../include/utility.h"

int sysPROGEND;
int sysSTACKSTART, sysSTACKEND;
int sysVARSTART, sysVAREND;
int sysGOSUBSTART, sysGOSUBEND;

const char string_0[] PROGMEM = "OK";
const char string_1[] PROGMEM = "Bad number";
const char string_2[] PROGMEM = "Line too long";
const char string_3[] PROGMEM = "Unexpected input";
const char string_4[] PROGMEM = "Unterminated string";
const char string_5[] PROGMEM = "Missing bracket";
const char string_6[] PROGMEM = "Error in expr";
const char string_7[] PROGMEM = "Numeric expr expected";
const char string_8[] PROGMEM = "String expr expected";
const char string_9[] PROGMEM = "Line number too big";
const char string_10[] PROGMEM = "Out of memory";
const char string_11[] PROGMEM = "Div by zero";
const char string_12[] PROGMEM = "Variable not found";
const char string_13[] PROGMEM = "Bad command";
const char string_14[] PROGMEM = "Bad line number";
const char string_15[] PROGMEM = "Break pressed";
const char string_16[] PROGMEM = "NEXT without FOR";
const char string_17[] PROGMEM = "STOP statement";
const char string_18[] PROGMEM = "Missing THEN in IF";
const char string_19[] PROGMEM = "RETURN without GOSUB";
const char string_20[] PROGMEM = "Wrong array dims";
const char string_21[] PROGMEM = "Bad array index";
const char string_22[] PROGMEM = "Bad string index";
const char string_23[] PROGMEM = "Error in VAL input";
const char string_24[] PROGMEM = "Bad parameter";

/* TODO PROGMEM const char *errorTable[] = { */
const char* const errorTable[] PROGMEM = {
    string_0, string_1, string_2, string_3,
    string_4, string_5, string_6, string_7,
    string_8, string_9, string_10, string_11,
    string_12, string_13, string_14, string_15,
    string_16, string_17, string_18, string_19,
    string_20, string_21, string_22, string_23,
    string_24
};

/************************* Token flags *************************/
/* Bits 1 + 2 number of arguments */
#define TKN_ARGS_NUM_MASK       0x03

/* Bit 3 return type (set if string) */
#define TKN_RET_TYPE_STR        0x04

/* Bits 4 - 6 argument type (set if string) */
#define TKN_ARG1_TYPE_STR       0x08
#define TKN_ARG2_TYPE_STR       0x10
#define TKN_ARG3_TYPE_STR       0x20

#define TKN_ARG_MASK            0x38
#define TKN_ARG_SHIFT           3

/* Bits 7, 8 formatting */
#define TKN_FMT_POST            0x40
#define TKN_FMT_PRE             0x80

/************************* Tokens *************************/
PROGMEM const TokenTableEntry tokenTable[] = {
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {"(", 0}, {")",0}, {"+",0}, {"-",0},
    {"*",0}, {"/",0}, {"=",0}, {">",0},
    {"<",0}, {"<>",0}, {">=",0}, {"<=",0},
    {":",TKN_FMT_POST}, {";",0}, {",",0}, {"AND",TKN_FMT_PRE|TKN_FMT_POST},
    {"OR",TKN_FMT_PRE|TKN_FMT_POST}, {"NOT",TKN_FMT_POST}, {"PRINT",TKN_FMT_POST}, {"LET",TKN_FMT_POST},
    {"LIST",TKN_FMT_POST}, {"RUN",TKN_FMT_POST}, {"GOTO",TKN_FMT_POST}, {"REM",TKN_FMT_POST},
    {"STOP",TKN_FMT_POST}, {"INPUT",TKN_FMT_POST}, {"CONT",TKN_FMT_POST}, {"IF",TKN_FMT_POST},
    {"THEN",TKN_FMT_PRE|TKN_FMT_POST}, {"LEN",1|TKN_ARG1_TYPE_STR}, {"VAL",1|TKN_ARG1_TYPE_STR}, {"RND",0},
    {"INT",1}, {"STR$", 1|TKN_RET_TYPE_STR}, {"FOR",TKN_FMT_POST}, {"TO",TKN_FMT_PRE|TKN_FMT_POST},
    {"STEP",TKN_FMT_PRE|TKN_FMT_POST}, {"NEXT", TKN_FMT_POST}, {"MOD",TKN_FMT_PRE|TKN_FMT_POST}, {"NEW",TKN_FMT_POST},
    {"GOSUB",TKN_FMT_POST}, {"RETURN",TKN_FMT_POST}, {"DIM", TKN_FMT_POST}, {"LEFT$",2|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR},
    {"RIGHT$",2|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR}, {"MID$",3|TKN_ARG1_TYPE_STR|TKN_RET_TYPE_STR}, {"CLS",TKN_FMT_POST},
    {"PAUSE",TKN_FMT_POST}, {"POSITION", TKN_FMT_POST}, {"PIN",TKN_FMT_POST}, {"PINMODE", TKN_FMT_POST}, {"INKEY$", 0},
    {"SAVE", TKN_FMT_POST}, {"LOAD", TKN_FMT_POST}, {"PINREAD",1}, {"ANALOGRD",1}, {"DIR", TKN_FMT_POST},
    {"DELETE", TKN_FMT_POST}, {"ABS",1}, {"SQR",1}, {"SIN",1}, {"COS",1}, {"TAN",1}
};

/* **************************************************************************
 * PROGRAM FUNCTIONS
 * **************************************************************************/
void printTokens(unsigned char *p) {
    int modeREM = 0;
    while (*p != TOKEN_EOL) {
        if (*p == TOKEN_IDENT) {
            p++;
            while (*p < 0x80)
                host_outputChar(*p++);
            host_outputChar((*p++)-0x80);
        }
        else if (*p == TOKEN_NUMBER) {
            p++;
            host_outputFloat(*(float*)p);
            p+=4;
        }
        else if (*p == TOKEN_INTEGER) {
            p++;
            host_outputInt(*(long*)p);
            p+=4;
        }
        else if (*p == TOKEN_STRING) {
            p++;
            if (modeREM) {
                host_outputString((char*)p);
                p+=1 + strlen((char*)p);
            }
            else {
                host_outputChar('\"');
                while (*p) {
                    if (*p == '\"') host_outputChar('\"');
                    host_outputChar(*p++);
                }
                host_outputChar('\"');
                p++;
            }
        }
        else {
            uint8_t fmt = pgm_read_byte_near(&tokenTable[*p].format);
            if (fmt & TKN_FMT_PRE) {
                host_outputChar(' ');
            }

            host_outputString((char *)pgm_read_word(&tokenTable[*p].token));
            if (fmt & TKN_FMT_POST) {
                host_outputChar(' ');
            }
            
            if (*p==TOKEN_REM) {
                modeREM = 1;
            }
            p++;
        }
    }
}

void listProg(uint16_t first, uint16_t last) {
    unsigned char *p = &mem[0];
    while (p < &mem[sysPROGEND]) {
        uint16_t lineNum = *(uint16_t*)(p+2);
        if ((!first || lineNum >= first) && (!last || lineNum <= last)) {
            host_outputInt(lineNum);
            host_outputChar(' ');
            printTokens(p+4);
            host_newLine();
        }

        p+= *(uint16_t *)p;
    }
}

unsigned char *findProgLine(uint16_t targetLineNumber) {
    unsigned char *p = &mem[0];
    while (p < &mem[sysPROGEND]) {
        uint16_t lineNum = *(uint16_t*)(p+2);
        if (lineNum >= targetLineNumber) {
            break;
        }
        p+= *(uint16_t *)p;
    }
    return p;
}

void deleteProgLine(unsigned char *p) {
    uint16_t lineLen = *(uint16_t*)p;
    sysPROGEND -= lineLen;
    memmove(p, p+lineLen, &mem[sysPROGEND] - p);
}

int doProgLine(uint16_t lineNum, unsigned char* tokenPtr, int tokensLength) {
    /* Find line of the at or immediately after the number */
    unsigned char *p = findProgLine(lineNum);
    uint16_t foundLine = 0;
    if (p < &mem[sysPROGEND]) {
        foundLine = *(uint16_t*)(p + 2);
    }

    /* If there's a line matching this one - delete it */
    if (foundLine == lineNum) {
        deleteProgLine(p);
    }

    /* Now check to see if this is an empty line, if so don't insert it */
    if (*tokenPtr == TOKEN_EOL) {
        return 1;
    }

    /* We now need to insert the new line at p */
    int bytesNeeded = 4 + tokensLength; /* length, linenum + tokens */
    if (sysPROGEND + bytesNeeded > sysVARSTART) {
        return 0;
    }

    /* Make room if this isn't the last line */
    if (foundLine) {
        memmove(p + bytesNeeded, p, &mem[sysPROGEND] - p);
    }

    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *(uint16_t *)p = lineNum; 
    p += 2;
    memcpy(p, tokenPtr, tokensLength);
    sysPROGEND += bytesNeeded;
    return 1;
}

/* **************************************************************************
 * CALCULATOR STACK FUNCTIONS
 * **************************************************************************/
/*
   Calculator stack starts at the start of memory after the program
   and grows towards the end
   contains either floats or null-terminated strings with the length on the end
*/
int stackPushNum(float val) {
    if (sysSTACKEND + (int)sizeof(float) > sysVARSTART) {
        return 0;   /* Out of memory */
    }

    unsigned char *p = &mem[sysSTACKEND];
    *(float *)p = val;
    sysSTACKEND += sizeof(float);
    return 1;
}

float stackPopNum(void) {
    sysSTACKEND -= sizeof(float);
    unsigned char *p = &mem[sysSTACKEND];
    return *(float *)p;
}

int stackPushStr(char *str) {
    int len = 1 + strlen(str);
    if (sysSTACKEND + len + 2 > sysVARSTART) {
        return 0;   /* Out of memory */
    }

    unsigned char *p = &mem[sysSTACKEND];
    strcpy((char*)p, str);
    p += len;
    *(uint16_t *)p = len;
    sysSTACKEND += len + 2;
    return 1;
}

char *stackGetStr(void) {
    /* Returns string without popping it */
    unsigned char *p = &mem[sysSTACKEND];
    int len = *(uint16_t *)(p - 2);
    return (char *)(p - len - 2);
}

char *stackPopStr(void) {
    unsigned char *p = &mem[sysSTACKEND];
    int len = *(uint16_t *)(p - 2);
    sysSTACKEND -= (len + 2);
    return (char *)&mem[sysSTACKEND];
}

void stackAdd2Strs(void) {
    /* Equivalent to popping 2 strings, concatenating them and pushing the result */
    unsigned char *p = &mem[sysSTACKEND];
    int str2len = *(uint16_t *)(p - 2);
    sysSTACKEND -= (str2len + 2);
    char *str2 = (char*)&mem[sysSTACKEND];
    p = &mem[sysSTACKEND];
    int str1len = *(uint16_t *)(p - 2);
    sysSTACKEND -= (str1len + 2);
    char *str1 = (char*)&mem[sysSTACKEND];
    p = &mem[sysSTACKEND];

    /* Shift the second string up (overwriting the null terminator of the first string) */
    memmove(str1 + str1len - 1, str2, str2len);

    /* Write the length and update stackend */
    int newLen = str1len + str2len - 1;
    p += newLen;
    *(uint16_t *)p = newLen;
    sysSTACKEND += newLen + 2;
}

/* mode 0 = LEFT$, 1 = RIGHT$ */
void stackLeftOrRightStr(int len, int mode) {
    /* Equivalent to popping the current string, doing the operation then pushing it again */
    unsigned char *p = &mem[sysSTACKEND];
    int strlen = *(uint16_t *)(p - 2);
    len++; /* Include trailing null */
    if (len > strlen) {
        len = strlen;
    }

    if (len == strlen) {
        return;  /* Nothing to do */
    }

    sysSTACKEND -= (strlen + 2);
    p = &mem[sysSTACKEND];
    if (mode == 0) {
        /* Truncate the string on the stack */
        *(p + len - 1) = 0;
    }
    else {
        /* Copy the rightmost characters */
        memmove(p, p + strlen - len, len);
    }

    /* Write the length and update stackend */
    p += len;
    *(uint16_t *)p = len;
    sysSTACKEND += len + 2;
}

void stackMidStr(int start, int len) {
    /* Equivalent to popping the current string, doing the operation then pushing it again */
    unsigned char *p = &mem[sysSTACKEND];
    int strlen = *(uint16_t *)(p - 2);
    len++; /* Include trailing null */
    if (start > strlen) {
        start = strlen;
    }

    start--;    /* Basic strings start at 1 */
    if (start + len > strlen) {
        len = strlen - start;
    }

    if (len == strlen) {
        return;  /* Nothing to do */
    }

    sysSTACKEND -= (strlen + 2);
    p = &mem[sysSTACKEND];

    /* Copy the characters */
    memmove(p, p + start, len - 1);
    *(p + len - 1) = 0;

    /* Write the length and update stackend */
    p += len;
    *(uint16_t *)p = len;
    sysSTACKEND += len + 2;
}

/* **************************************************************************
 * VARIABLE TABLE FUNCTIONS
 * **************************************************************************/
/*
   Variable table starts at the end of memory and grows towards the start
   Simple variable
   table +--------+-------+-----------------+-----------------+ . . .
    <--- | len    | type  | name            | value           |
   grows | 2bytes | 1byte | null terminated | float/string    | 
         +--------+-------+-----------------+-----------------+ . . .
  
   Array
   +--------+-------+-----------------+----------+-------+ . . .+-------+-------------+. . 
   | len    | type  | name            | num dims | dim1  |      | dimN  | elem(1,..1) |
   | 2bytes | 1byte | null terminated | 2bytes   | 2bytes|      | 2bytes| float       |
   +--------+-------+-----------------+----------+-------+ . . .+-------+-------------+. . 
*/

/* Variable type byte */
#define VAR_TYPE_NUM            0x1
#define VAR_TYPE_FORNEXT        0x2
#define VAR_TYPE_NUM_ARRAY      0x4
#define VAR_TYPE_STRING         0x8
#define VAR_TYPE_STR_ARRAY      0x10

unsigned char *findVariable(char *searchName, int searchMask) {
    unsigned char *p = &mem[sysVARSTART];
    while (p < &mem[sysVAREND]) {
        int type = *(p + 2);
        if (type & searchMask) {
            unsigned char *name = p+3;
            if (strcasecmp((char*)name, searchName) == 0) {
                return p;
            }
        }
        p += *(uint16_t *)p;
    }
    return NULL;
}

void deleteVariableAt(unsigned char *pos) {
    int len = *(uint16_t *)pos;
    if (pos == &mem[sysVARSTART]) {
        sysVARSTART += len;
        return;
    }
    memmove(&mem[sysVARSTART] + len, &mem[sysVARSTART], pos - &mem[sysVARSTART]);
    sysVARSTART += len;
}

/* TODO: consistently return errors rather than 1 or 0? */
int storeNumVariable(char *name, float val) {
    /* These can be modified in place */
    int nameLen = strlen(name);
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);

    if (p != NULL) {   
        /* Replace the old value (could either be VAR_TYPE_NUM or VAR_TYPE_FORNEXT) */
        p += 3; /* len + type; */
        p += nameLen + 1;
        *(float *)p = val;
    }
    else {   
        /* Allocate a new variable */
        int bytesNeeded = 3;  /* len + flags */
        bytesNeeded += nameLen + 1;     /* name */
        bytesNeeded += sizeof(float);   /* val */

        if (sysVARSTART - bytesNeeded < sysSTACKEND) {
            return 0;   /* Out of memory */
        }

        sysVARSTART -= bytesNeeded;
        p = &mem[sysVARSTART];
        *(uint16_t *)p = bytesNeeded; 
        p += 2;
        *p++ = VAR_TYPE_NUM;
        strcpy((char*)p, name); 
        p += nameLen + 1;
        *(float *)p = val;
    }
    return 1;
}

int storeForNextVariable(char *name, float start, float step, float end, uint16_t lineNum, uint16_t stmtNum) {
    int nameLen = strlen(name);
    int bytesNeeded = 3;                /* len + flags */
    bytesNeeded += nameLen + 1;         /* name */
    bytesNeeded += 3 * sizeof(float);   /* vals */
    bytesNeeded += 2 * sizeof(uint16_t);

    /* Unlike simple numeric variables, these are reallocated if they already exist
       since the existing value might be a simple variable or a for/next variable */
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);
    if (p != NULL) {
        /* Check there will actually be room for the new value */
        uint16_t oldVarLen = *(uint16_t*)p;
        if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND) {
            return 0;   /* Not enough memory */
        }

        deleteVariableAt(p);
    }

    if (sysVARSTART - bytesNeeded < sysSTACKEND) {
        return 0;   /* Out of memory */
    }

    sysVARSTART -= bytesNeeded;

    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = VAR_TYPE_FORNEXT;
    strcpy((char*)p, name); 
    p += nameLen + 1;
    *(float *)p = start; 
    p += sizeof(float);
    *(float *)p = step; 
    p += sizeof(float);
    *(float *)p = end; 
    p += sizeof(float);
    *(uint16_t *)p = lineNum; 
    p += sizeof(uint16_t);
    *(uint16_t *)p = stmtNum;
    return 1;
}

int storeStrVariable(char *name, char *val) {
    int nameLen = strlen(name);
    int valLen = strlen(val);
    int bytesNeeded = 3;        /* len + type */
    bytesNeeded += nameLen + 1; /* name */
    bytesNeeded += valLen + 1;  /* val */

    /* Strings and arrays are re-allocated if they already exist */
    unsigned char *p = findVariable(name, VAR_TYPE_STRING);
    if (p != NULL) {
        /* Check there will actually be room for the new value */
        uint16_t oldVarLen = *(uint16_t*)p;
        if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND) {
            return 0;   /* Not enough memory */
        }

        deleteVariableAt(p);
    }

    if (sysVARSTART - bytesNeeded < sysSTACKEND) {
        return 0;   /* Out of memory */
    }

    sysVARSTART -= bytesNeeded;

    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = VAR_TYPE_STRING;
    strcpy((char*)p, name); 
    p += nameLen + 1;
    strcpy((char*)p, val);
    return 1;
}

int createArray(char *name, int isString) {
    /* Dimensions and number of dimensions on the calculator stack */
    int nameLen = strlen(name);
    int bytesNeeded = 3;        /* len + flags */
    bytesNeeded += nameLen + 1; /* name */
    bytesNeeded += 2;           /* num dims */
    int numElements = 1;
    int i = 0;
    int numDims = (int)stackPopNum();

    /* keep the current stack position, since we'll need to pop these values again */
    int oldSTACKEND = sysSTACKEND;  
    for (i=0; i<numDims; i++) {
        int dim = (int)stackPopNum();
        numElements *= dim;
    }

    bytesNeeded += 2 * numDims + (isString ? 1 : sizeof(float)) * numElements;
    
    /* Strings and arrays are re-allocated if they already exist */
    unsigned char *p = findVariable(name, (isString ? VAR_TYPE_STR_ARRAY : VAR_TYPE_NUM_ARRAY));
    if (p != NULL) {
        /* Check there will actually be room for the new value */
        uint16_t oldVarLen = *(uint16_t*)p;
        if (sysVARSTART - (bytesNeeded - oldVarLen) < sysSTACKEND) {
            return 0;   /* Not enough memory */
        }

        deleteVariableAt(p);
    }

    if (sysVARSTART - bytesNeeded < sysSTACKEND) {
        return 0;   /* Out of memory */
    }

    sysVARSTART -= bytesNeeded;

    p = &mem[sysVARSTART];
    *(uint16_t *)p = bytesNeeded; 
    p += 2;
    *p++ = (isString ? VAR_TYPE_STR_ARRAY : VAR_TYPE_NUM_ARRAY);
    strcpy((char*)p, name); 
    p += nameLen + 1;
    *(uint16_t *)p = numDims; 
    p += 2;
    sysSTACKEND = oldSTACKEND;
    for (i=0; i<numDims; i++) {
        int dim = (int)stackPopNum();
        *(uint16_t *)p = dim; 
        p += 2;
    }

    memset(p, 0, numElements * (isString ? 1 : sizeof(float)));
    return 1;
}

int _getArrayElemOffset(unsigned char **p, int *pOffset) {
    /* Check for correct dimensionality */
    int numArrayDims = *(uint16_t*)*p; 
    *p += 2;
    int numDimsGiven = (int)stackPopNum();
    if (numArrayDims != numDimsGiven) {
        return ERROR_WRONG_ARRAY_DIMENSIONS;
    }

    /* Now lookup the element */
    int offset = 0;
    int base = 1;
    for (int i = 0; i < numArrayDims; i++) {
        int index = (int)stackPopNum();
        int arrayDim = *(uint16_t*)*p; 
        *p += 2;
        if (index < 1 || index > arrayDim) {
            return ERROR_ARRAY_SUBSCRIPT_OUT_RANGE;
        }

        offset += base * (index - 1);
        base *= arrayDim;
    }
    *pOffset = offset;
    return 0;
}

int setNumArrayElem(char *name, float val) {
    /* Each index and number of dimensions on the calculator stack */
    unsigned char *p = findVariable(name, VAR_TYPE_NUM_ARRAY);
    if (p == NULL) {
        return ERROR_VARIABLE_NOT_FOUND;
    }

    p += 3 + strlen(name) + 1;
    
    int offset;
    int ret = _getArrayElemOffset(&p, &offset);
    if (ret) {
        return ret;
    }

    p += sizeof(float)*offset;
    *(float *)p = val;
    return ERROR_NONE;
}

int setStrArrayElem(char *name) {
    /* string is top of the stack each index and number of dimensions
       on the calculator stack */

    /* Keep the current stack position, since we can't overwrite the value string */
    int oldSTACKEND = sysSTACKEND;

    /* How long is the new value? */
    char *newValPtr = stackPopStr();
    int newValLen = strlen(newValPtr);

    unsigned char *p = findVariable(name, VAR_TYPE_STR_ARRAY);
    unsigned char *p1 = p;  /* So we can correct the length when done */
    if (p == NULL) {
        return ERROR_VARIABLE_NOT_FOUND;
    }

    p += 3 + strlen(name) + 1;
    
    int offset;
    int ret = _getArrayElemOffset(&p, &offset);
    if (ret) {
        return ret;
    }

    /* Find the correct element by skipping over null terminators */
    int i = 0;
    while (i < offset) {
        if (*p == 0) i++;
        p++;
    }

    int oldValLen = strlen((char*)p);
    int bytesNeeded = newValLen - oldValLen;
    
    /* Check if we've got enough room for the new value */
    if (sysVARSTART - bytesNeeded < oldSTACKEND) {
        return 0;   /* Out of memory */
    }

    // correct the length of the variable
    *(uint16_t*)p1 += bytesNeeded;
    memmove(&mem[sysVARSTART - bytesNeeded], &mem[sysVARSTART], p - &mem[sysVARSTART]);

    /* Copy in the new value */
    strcpy((char*)(p - bytesNeeded), newValPtr);
    sysVARSTART -= bytesNeeded;
    return ERROR_NONE;
}

float lookupNumArrayElem(char *name, int *error) {
    /* Each index and number of dimensions on the calculator stack */
    unsigned char *p = findVariable(name, VAR_TYPE_NUM_ARRAY);
    if (p == NULL) {
        *error = ERROR_VARIABLE_NOT_FOUND;
        return 0.0f;
    }
    p += 3 + strlen(name) + 1;
    
    int offset;
    int ret = _getArrayElemOffset(&p, &offset);
    if (ret) {
        *error = ret;
        return 0.0f;
    }
    p += sizeof(float)*offset;
    return *(float *)p;
}

char *lookupStrArrayElem(char *name, int *error) {
    /* Each index and number of dimensions on the calculator stack */
    unsigned char *p = findVariable(name, VAR_TYPE_STR_ARRAY);
    if (p == NULL) {
        *error = ERROR_VARIABLE_NOT_FOUND;
        return NULL;
    }
    p += 3 + strlen(name) + 1;

    int offset;
    int ret = _getArrayElemOffset(&p, &offset);
    if (ret) {
        *error = ret;
        return NULL;
    }

    /* Find the correct element by skipping over null terminators */
    int i = 0;
    while (i < offset) {
        if (*p == 0) i++;
        p++;
    }
    return (char *)p;
}

float lookupNumVariable(char *name) {
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);
    if (p == NULL) {
        return FLT_MAX;
    }
    p += 3 + strlen(name) + 1;
    return *(float *)p;
}

char *lookupStrVariable(char *name) {
    unsigned char *p = findVariable(name, VAR_TYPE_STRING);
    if (p == NULL) {
        return NULL;
    }
    p += 3 + strlen(name) + 1;
    return (char *)p;
}

ForNextData lookupForNextVariable(char *name) {
    ForNextData ret;
    unsigned char *p = findVariable(name, VAR_TYPE_NUM|VAR_TYPE_FORNEXT);
    if (p == NULL) {
        ret.val = FLT_MAX;
    }
    else if (*(p + 2) != VAR_TYPE_FORNEXT) {
        ret.step = FLT_MAX;
    }
    else {
        p += 3 + strlen(name) + 1;
        ret.val = *(float *)p; 
        p += sizeof(float);
        ret.step = *(float *)p; 
        p += sizeof(float);
        ret.end = *(float *)p; 
        p += sizeof(float);
        ret.lineNumber = *(uint16_t *)p; 
        p += sizeof(uint16_t);
        ret.stmtNumber = *(uint16_t *)p;
    }

    return ret;
}

/* **************************************************************************
 * GOSUB STACK
 * **************************************************************************/
/* gosub stack (if used) is after the variables
   int gosubStackPush(int lineNumber,int stmtNumber) { TODO */
int gosubStackPush(int lineNum,int stmtNum) {
    int bytesNeeded = 2 * sizeof(uint16_t);
    if (sysVARSTART - bytesNeeded < sysSTACKEND) {
        return 0;   /* Out of memory */
    }

    /* Shift the variable table */
    memmove(&mem[sysVARSTART] - bytesNeeded, &mem[sysVARSTART], sysVAREND - sysVARSTART);
    sysVARSTART -= bytesNeeded;
    sysVAREND -= bytesNeeded;

    /* Push the return address */
    sysGOSUBSTART = sysVAREND;
    uint16_t *p = (uint16_t*)&mem[sysGOSUBSTART];
    *p++ = (uint16_t)lineNum;
    *p = (uint16_t)stmtNum;
    return 1;
}

/* int gosubStackPop(int *lineNumber, int *stmtNumber) {  TODO */
int gosubStackPop(int *lineNum, int *stmtNum) {
    if (sysGOSUBSTART == sysGOSUBEND) {
        return 0;
    }

    uint16_t *p = (uint16_t*)&mem[sysGOSUBSTART];

    /* TODO   *lineNumber = (int)*p++;
      *stmtNumber = (int)*p; */
    *lineNum = (int)*p++;
    *stmtNum = (int)*p;

    int bytesFreed = 2 * sizeof(uint16_t);
  
    /* Shift the variable table */
    memmove(&mem[sysVARSTART] + bytesFreed, &mem[sysVARSTART], sysVAREND - sysVARSTART);
    sysVARSTART += bytesFreed;
    sysVAREND += bytesFreed;
    sysGOSUBSTART = sysVAREND;
    return 1;
}

/* **************************************************************************
 * LEXER
 * **************************************************************************/

static unsigned char *tokenIn, *tokenOut;
static int tokenOutLeft;

/* nextToken returns -1 for end of input, 0 for success, +ve number = error code */
int nextToken(void) {
    /* Skip any whitespace */
    while (isspace(*tokenIn)) {
        tokenIn++;
    }

    /* Check for end of line */
    if (*tokenIn == 0) {
        *tokenOut++ = TOKEN_EOL;
        tokenOutLeft--;
        return -1;
    }

    /* Number: [0-9.]+
       TODO - handle 1e4 etc */
    if (isdigit(*tokenIn) || *tokenIn == '.') {  /* Number: [0-9.]+ */
        int gotDecimal = 0;
        char numStr[MAX_NUMBER_LEN+1];
        int numLen = 0;
        do {
            if (numLen == MAX_NUMBER_LEN) {
                return ERROR_LEXER_BAD_NUM;
            }

            if (*tokenIn == '.') {
                if (gotDecimal) {
                    return ERROR_LEXER_BAD_NUM;
                }
                else {
                    gotDecimal = 1;
                }
            }
            numStr[numLen++] = *tokenIn++;
        } 
        while (isdigit(*tokenIn) || *tokenIn == '.');

        numStr[numLen] = 0;
        if (tokenOutLeft <= 5) {
            return ERROR_LEXER_TOO_LONG;
        }

        tokenOutLeft -= 5;
        if (!gotDecimal) {
            long val = strtol(numStr, 0, 10);
            if (val == LONG_MAX || val == LONG_MIN) {
                gotDecimal = true;
            }
            else {
                *tokenOut++ = TOKEN_INTEGER;
                *(long*)tokenOut = (long)val;
                tokenOut += sizeof(long);
            }
        }

        if (gotDecimal) {
            *tokenOut++ = TOKEN_NUMBER;
            *(float*)tokenOut = (float)strtod(numStr, 0);
            tokenOut += sizeof(float);
        }
        return 0;
    }

    /* identifier: [a-zA-Z][a-zA-Z0-9]*[$] */
    if (isalpha(*tokenIn)) {
        char identStr[MAX_IDENT_LEN + 1];
        int identLen = 0;
        identStr[identLen++] = *tokenIn++; /* Copy first char */
        while (isalnum(*tokenIn) || *tokenIn=='$') {
            if (identLen < MAX_IDENT_LEN) {
                identStr[identLen++] = *tokenIn;
            }

            tokenIn++;
        }

        identStr[identLen] = 0;
        
        /* Check to see if this is a keyword */
        for (int i = FIRST_IDENT_TOKEN; i <= LAST_IDENT_TOKEN; i++) {
            if (strcasecmp(identStr, (char *)pgm_read_word(&tokenTable[i].token)) == 0) {
                if (tokenOutLeft <= 1) {
                    return ERROR_LEXER_TOO_LONG;
                }

                tokenOutLeft--;
                *tokenOut++ = i;
                
                /* Special case for REM */
                if (i == TOKEN_REM) {
                    *tokenOut++ = TOKEN_STRING;

                    /* Skip whitespace */
                    while (isspace(*tokenIn)) {
                        tokenIn++;
                    }

                    /* Copy the comment */
                    while (*tokenIn) {
                        *tokenOut++ = *tokenIn++;
                    }

                    *tokenOut++ = 0;
                }
                return 0;
            }
        }

        /* no matching keyword - this must be an identifier
           $ is only allowed at the end */
        char *dollarPos = strchr(identStr, '$');
        if  (dollarPos && dollarPos!= &identStr[0] + identLen - 1) {
            return ERROR_LEXER_UNEXPECTED_INPUT;
        }

        if (tokenOutLeft <= 1+identLen) {
            return ERROR_LEXER_TOO_LONG;
        }

        tokenOutLeft -= 1 + identLen;
        *tokenOut++ = TOKEN_IDENT;
        strcpy((char*)tokenOut, identStr);
        tokenOut[identLen-1] |= 0x80;
        tokenOut += identLen;
        return 0;
    }

    /* String */
    if (*tokenIn=='\"') {
        *tokenOut++ = TOKEN_STRING;
        tokenOutLeft--;
        if (tokenOutLeft <= 1) {
            return ERROR_LEXER_TOO_LONG;
        }

        tokenIn++;
        while (*tokenIn) {
            if (*tokenIn == '\"' && *(tokenIn+1) != '\"') {
                break;
            }
            else if (*tokenIn == '\"') {
                tokenIn++;
            }
            
            *tokenOut++ = *tokenIn++;
            tokenOutLeft--;
            if (tokenOutLeft <= 1) {
                return ERROR_LEXER_TOO_LONG;
            }
        }

        if (!*tokenIn) {
            return ERROR_LEXER_UNTERMINATED_STRING;
        }

        tokenIn++;
        *tokenOut++ = 0;
        tokenOutLeft--;
        return 0;
    }

    /* Handle non-alpha tokens e.g. = */
    for (int i = LAST_NON_ALPHA_TOKEN; i >= FIRST_NON_ALPHA_TOKEN; i--) {
        /* Do this "backwards" so we match >= correctly, not as > then = */
        int len = strlen((char *)pgm_read_word(&tokenTable[i].token));
        if (strncmp((char *)pgm_read_word(&tokenTable[i].token), (char*)tokenIn, len) == 0) {
            if (tokenOutLeft <= 1) {
                return ERROR_LEXER_TOO_LONG;
            }

            *tokenOut++ = i;
            tokenOutLeft--;
            tokenIn += len;
            return 0;
        }
    }
    return ERROR_LEXER_UNEXPECTED_INPUT;
}

int tokenize(unsigned char *input, unsigned char *output, int outputSize)
{
    tokenIn = input;
    tokenOut = output;
    tokenOutLeft = outputSize;
    int ret;
    while (1) {
        ret = nextToken();
        if (ret) {
            break;
        }
    }
    return (ret > 0) ? ret : 0;
}

/* **************************************************************************
 * PARSER / INTERPRETER
 * **************************************************************************/

static char executeMode;    /* 0 = syntax check only, 1 = execute */
uint16_t lineNumber, stmtNumber;

/* stmt number is 0 for the first statement, then increases after each command seperator (:)
   Note that IF a=1 THEN PRINT "x": print "y" is considered to be only 2 statements */
static uint16_t jumpLineNumber, jumpStmtNumber;
static uint16_t stopLineNumber, stopStmtNumber;
static char breakCurrentLine;

static unsigned char *tokenBuffer, *prevToken;
static int curToken;
static char identVal[MAX_IDENT_LEN+1];
static char isStrIdent;
static float numVal;
static char *strVal;
/* TODO remove static long numIntVal; */

int getNextToken(void) {
    prevToken = tokenBuffer;
    curToken = *tokenBuffer++;
    if (curToken == TOKEN_IDENT) {
        int i=0;
        while (*tokenBuffer < 0x80) {
            identVal[i++] = *tokenBuffer++;
        }

        identVal[i] = (*tokenBuffer++) - 0x80;
        isStrIdent = (identVal[i++] == '$');
        identVal[i++] = '\0';
    }
    else if (curToken == TOKEN_NUMBER) {
        numVal = *(float*)tokenBuffer;
        tokenBuffer += sizeof(float);
    }
    else if (curToken == TOKEN_INTEGER) {
        /* These are really just for line numbers */
        numVal = (float)(*(long*)tokenBuffer);
        tokenBuffer += sizeof(long);
    }
    else if (curToken == TOKEN_STRING) {
        strVal = (char*)tokenBuffer;
        tokenBuffer += 1 + strlen(strVal);
    }

    return curToken;
}

/* Value (int) returned from parseXXXXX */
#define ERROR_MASK                      0x0FFF
#define TYPE_MASK                       0xF000
#define TYPE_NUMBER                     0x0000
#define TYPE_STRING                     0x1000

#define IS_TYPE_NUM(x) ((x & TYPE_MASK) == TYPE_NUMBER)
#define IS_TYPE_STR(x) ((x & TYPE_MASK) == TYPE_STRING)

/* Parse a number */
int parseNumberExpr(void) {
    if (executeMode && !stackPushNum(numVal)) {
        return ERROR_OUT_OF_MEMORY;
    }

    getNextToken(); /* Consume the number */
    return TYPE_NUMBER;
}

/* Parse (x1,....xn) e.g. DIM a(10,20) */
int parseSubscriptExpr(void) {
    /* Stacks x1, .. xn followed by n */
    int numDims = 0;
    if (curToken != TOKEN_LBRACKET) {
        return ERROR_EXPR_MISSING_BRACKET;
    }

    getNextToken();
    while(1) {
        numDims++;
        int val = expectNumber();
        if (val) {
            return val;    /* error */
        }

        if (curToken == TOKEN_RBRACKET) {
            break;
        }
        else if (curToken == TOKEN_COMMA) {
            getNextToken();
        }
        else {
            return ERROR_EXPR_MISSING_BRACKET;
        }
    }

    getNextToken(); /* eat ) */
    if (executeMode && !stackPushNum(numDims)) {
        return ERROR_OUT_OF_MEMORY;
    }

    return 0;
}

/* Parse a function call e.g. LEN(a$) */
int parseFnCallExpr(void) {
    int op = curToken;
    int fnSpec = pgm_read_byte_near(&tokenTable[curToken].format);
    getNextToken();

    /* Get the required arguments and types from the token table */
    if (curToken != TOKEN_LBRACKET) {
        return ERROR_EXPR_MISSING_BRACKET;
    }

    getNextToken();

    int reqdArgs = fnSpec & TKN_ARGS_NUM_MASK;
    int argTypes = (fnSpec & TKN_ARG_MASK) >> TKN_ARG_SHIFT;
    int ret = (fnSpec & TKN_RET_TYPE_STR) ? TYPE_STRING : TYPE_NUMBER;
    for (int i=0; i<reqdArgs; i++) {
        int val = parseExpression();
        if (val & ERROR_MASK) return val;

        /* Check we've got the right type */
        if (!(argTypes & 1) && !IS_TYPE_NUM(val)) {
            return ERROR_EXPR_EXPECTED_NUM;
        }

        if ((argTypes & 1) && !IS_TYPE_STR(val)) {
            return ERROR_EXPR_EXPECTED_STR;
        }

        argTypes >>= 1;

        /* If this isn't the last argument, eat the , */
        if (i+1<reqdArgs) {
            if (curToken != TOKEN_COMMA) {
                return ERROR_UNEXPECTED_TOKEN;
            }

            getNextToken();
        }
    }

    /* Now all the arguments will be on the stack (last first) */
    if (executeMode) {
        int tmp;
        switch (op) {
        case TOKEN_INT:
            stackPushNum((float)floor(stackPopNum()));
            break;
        case TOKEN_ABS:
            DEBUG_SERIAL_PRINT("TOKEN_ABS");
            stackPushNum((float)fabs(stackPopNum()));
            break;
        case TOKEN_SQR:
            DEBUG_SERIAL_PRINT("TOKEN_SQR");
            stackPushNum((float)sqrt(stackPopNum()));
            break;
        case TOKEN_SIN:
            DEBUG_SERIAL_PRINT("TOKEN_SIN");
            stackPushNum((float)sin(stackPopNum()));
            break;
        case TOKEN_COS:
            DEBUG_SERIAL_PRINT("TOKEN_COS");
            stackPushNum((float)cos(stackPopNum()));
            break;
        case TOKEN_TAN:
            DEBUG_SERIAL_PRINT("TOKEN_TAN");
            stackPushNum((float)tan(stackPopNum()));
            break;
        case TOKEN_STR:
            {
                char buf[16];
                if (!stackPushStr(host_floatToStr(stackPopNum(), buf))) {
                    return ERROR_OUT_OF_MEMORY;
                }
            }
            break;
        case TOKEN_LEN:
            tmp = strlen(stackPopStr());
            if (!stackPushNum(tmp)) {
                return ERROR_OUT_OF_MEMORY;
            }
            break;
        case TOKEN_VAL:
            {
                // tokenise str onto the stack
                int oldStackEnd = sysSTACKEND;
                unsigned char *oldTokenBuffer = prevToken;
                int val = tokenize((unsigned char*)stackGetStr(), &mem[sysSTACKEND], sysVARSTART - sysSTACKEND);
                if (val) {
                    if (val == ERROR_LEXER_TOO_LONG) return ERROR_OUT_OF_MEMORY;
                    else return ERROR_IN_VAL_INPUT;
                }

                /* Set tokenBuffer to point to the new set of tokens on the stack */
                tokenBuffer = &mem[sysSTACKEND];

                /* Move stack end to the end of the new tokens */
                sysSTACKEND = tokenOut - &mem[0];
                getNextToken();

                /* Then parseExpression */
                val = parseExpression();
                if (val & ERROR_MASK) {
                    if (val == ERROR_OUT_OF_MEMORY) return val;
                    else return ERROR_IN_VAL_INPUT;
                }

                if (!IS_TYPE_NUM(val)) {
                    return ERROR_EXPR_EXPECTED_NUM;
                }

                /* Read the result from the stack */
                float f = stackPopNum();

                /* Pop the tokens from the stack */
                sysSTACKEND = oldStackEnd;

                /* And pop the original string */
                stackPopStr();

                /* Finally, push the result and set the token buffer back */
                stackPushNum(f);
                tokenBuffer = oldTokenBuffer;
                getNextToken();
            }
            break;
        case TOKEN_LEFT:
            tmp = (int)stackPopNum();
            if (tmp < 0) {
                return ERROR_STR_SUBSCRIPT_OUT_RANGE;
            }
            stackLeftOrRightStr(tmp, 0);
            break;
        case TOKEN_RIGHT:
            tmp = (int)stackPopNum();
            if (tmp < 0) {
                return ERROR_STR_SUBSCRIPT_OUT_RANGE;
            }
            stackLeftOrRightStr(tmp, 1);
            break;
        case TOKEN_MID:
            {
                tmp = (int)stackPopNum();
                int start = stackPopNum();
                if (tmp < 0 || start < 1) {
                    return ERROR_STR_SUBSCRIPT_OUT_RANGE;
                }
                stackMidStr(start, tmp);
            }
            break;
        case TOKEN_PINREAD:
            tmp = (int)stackPopNum();
            if (!stackPushNum(host_digitalRead(tmp))) {
                return ERROR_OUT_OF_MEMORY;
            }
            break;
        case TOKEN_ANALOGRD:
            tmp = (int)stackPopNum();
            if (!stackPushNum(host_analogRead(tmp))) {
                return ERROR_OUT_OF_MEMORY;
            }
            break;
        default:
            return ERROR_UNEXPECTED_TOKEN;
        }
    }

    if (curToken != TOKEN_RBRACKET) {
        return ERROR_EXPR_MISSING_BRACKET;
    }

    getNextToken(); /* eat ) */
    return ret;
}

/* Parse an identifer e.g. a$ or a(5,3) */
int parseIdentifierExpr(void) {
    char ident[MAX_IDENT_LEN + 1];
    if (executeMode) {
        strcpy(ident, identVal);
    }

    int isStringIdentifier = isStrIdent;
    getNextToken(); /* Eat ident */
    if (curToken == TOKEN_LBRACKET) {
        /* Array access */
        int val = parseSubscriptExpr();
        if (val) return val;
        if (executeMode) {
            if (isStringIdentifier) {
                int error = 0;
                char *str = lookupStrArrayElem(ident, &error);
                if (error) {
                    return error;
                }
                else if (!stackPushStr(str)) {
                    return ERROR_OUT_OF_MEMORY;
                }
            }
            else {
                int error = 0;
                float f = lookupNumArrayElem(ident, &error);
                if (error) {
                    return error;
                }
                else if (!stackPushNum(f)) {
                    return ERROR_OUT_OF_MEMORY;
                }
            }
        }
    }
    else {
        /* Simple variable */
        if (executeMode) {
            if (isStringIdentifier) {
                char *str = lookupStrVariable(ident);
                if (!str) {
                    return ERROR_VARIABLE_NOT_FOUND;
                }
                else if (!stackPushStr(str)) {
                    return ERROR_OUT_OF_MEMORY;
                }
            }
            else {
                float f = lookupNumVariable(ident);
                if (f == FLT_MAX) {
                    return ERROR_VARIABLE_NOT_FOUND;
                }
                else if (!stackPushNum(f)) {
                    return ERROR_OUT_OF_MEMORY;
                }
            }
        }
    }
    return isStringIdentifier ? TYPE_STRING : TYPE_NUMBER;
}

/* Parse a string e.g. "hello" */
int parseStringExpr(void) {
    if (executeMode && !stackPushStr(strVal)) {
        return ERROR_OUT_OF_MEMORY;
    }

    getNextToken(); /* Consume the string */
    return TYPE_STRING;
}

/* Parse a bracketed expressed e.g. (5+3) */
int parseParenExpr(void) {
    getNextToken();  /* eat ( */
    int val = parseExpression();
    if (val & ERROR_MASK) {
        return val;
    }

    if (curToken != TOKEN_RBRACKET) {
        return ERROR_EXPR_MISSING_BRACKET;
    }
    getNextToken();  /* eat ) */
    return val;
}

int parse_RND(void) {
    getNextToken();
    if (executeMode && !stackPushNum((float)rand()/(float)RAND_MAX)) {
        return ERROR_OUT_OF_MEMORY;
    }
    return TYPE_NUMBER; 
}

int parse_INKEY(void) {
    getNextToken();
    if (executeMode) {
        char str[2];
        str[0] = host_getKey();
        str[1] = 0;
        if (!stackPushStr(str)) {
            return ERROR_OUT_OF_MEMORY;
        }
    }
    return TYPE_STRING; 
}

int parseUnaryNumExp(void) {
    int op = curToken;
    getNextToken();
    int val = parsePrimary();
    if (val & ERROR_MASK) {
        return val;
    }

    if (!IS_TYPE_NUM(val)) {
        return ERROR_EXPR_EXPECTED_NUM;
    }

    switch (op) {
    case TOKEN_MINUS:
        if (executeMode) {
            stackPushNum(stackPopNum() * -1.0f);
        }
        return TYPE_NUMBER;
    case TOKEN_NOT:
        if (executeMode) {
            stackPushNum(stackPopNum() ? 0.0f : 1.0f);
        }
        return TYPE_NUMBER;
    default:
        return ERROR_UNEXPECTED_TOKEN;
    }
}

/* Primary */
int parsePrimary() {
    switch (curToken) {
    case TOKEN_IDENT:   
        return parseIdentifierExpr();
    case TOKEN_NUMBER:
    case TOKEN_INTEGER:
        return parseNumberExpr();
    case TOKEN_STRING:  
        return parseStringExpr();
    case TOKEN_LBRACKET:
        return parseParenExpr();

        /* "psuedo-identifiers" */
    case TOKEN_RND: 
        return parse_RND();
    case TOKEN_INKEY:
        return parse_INKEY();

        /* unary ops */
    case TOKEN_MINUS:
    case TOKEN_NOT:
        return parseUnaryNumExp();

        /* functions */
    case TOKEN_INT: 
    case TOKEN_STR: 
    case TOKEN_LEN: 
    case TOKEN_VAL:
    case TOKEN_LEFT: 
    case TOKEN_RIGHT: 
    case TOKEN_MID: 
    case TOKEN_PINREAD:
    case TOKEN_ANALOGRD:
    case TOKEN_ABS:
    case TOKEN_SQR:
    case TOKEN_SIN:
    case TOKEN_COS:
    case TOKEN_TAN:
        return parseFnCallExpr();

    default:
        return ERROR_UNEXPECTED_TOKEN;
    }
}

int getTokPrecedence(void) {
    if (curToken == TOKEN_AND || curToken == TOKEN_OR) {
        return 5;
    }

    if (curToken == TOKEN_EQUALS || curToken == TOKEN_NOT_EQ) {
        return 10;
    }

    if (curToken == TOKEN_LT || curToken == TOKEN_GT || curToken == TOKEN_LT_EQ || curToken == TOKEN_GT_EQ) {
        return 20;
    }

    if (curToken == TOKEN_MINUS || curToken == TOKEN_PLUS) {
        return 30;
    }
    else if (curToken == TOKEN_MULT || curToken == TOKEN_DIV || curToken == TOKEN_MOD) {
        return 40;
    }
    else {
        return -1;
    }
}

/* Operator-Precedence Parsing */
int parseBinOpRHS(int ExprPrec, int lhsVal) {
    /* If this is a binop, find its precedence */
    while (1) {
        int TokPrec = getTokPrecedence();

        /* If this is a binop that binds at least as tightly as the current binop,
           consume it, otherwise we are done */
        if (TokPrec < ExprPrec) {
            return lhsVal;
        }

        /* Okay, we know this is a binop */
        int BinOp = curToken;
        getNextToken();  /* eat binop */

        /* Parse the primary expression after the binary operator */
        int rhsVal = parsePrimary();
        if (rhsVal & ERROR_MASK) {
            return rhsVal;
        }

        /* If BinOp binds less tightly with RHS than the operator after RHS, let
           the pending operator take RHS as its LHS */
        int NextPrec = getTokPrecedence();
        if (TokPrec < NextPrec) {
            rhsVal = parseBinOpRHS(TokPrec + 1, rhsVal);
            if (rhsVal & ERROR_MASK) return rhsVal;
        }

        if (IS_TYPE_NUM(lhsVal) && IS_TYPE_NUM(rhsVal)) {   
            /* Number operations */
            float r = 0, l = 0;
            if (executeMode) {
                r = stackPopNum();
                l = stackPopNum();
            }

            if (BinOp == TOKEN_PLUS) {
                if (executeMode) {
                    stackPushNum(l + r);
                }
            }
            else if (BinOp == TOKEN_MINUS) {
                if (executeMode) {
                    stackPushNum(l - r);
                }
            }
            else if (BinOp == TOKEN_MULT) {
                if (executeMode) {
                    stackPushNum(l * r);
                }
            }
            else if (BinOp == TOKEN_DIV) {
                if (executeMode) {
                    if (r) {
                        stackPushNum(l / r);
                    }
                    else {
                        return ERROR_EXPR_DIV_ZERO;
                    }
                }
            }
            else if (BinOp == TOKEN_MOD) {
                if (executeMode) {
                    if ((int)r) {
                        stackPushNum((float)((int)l % (int)r));
                    }
                    else {
                        return ERROR_EXPR_DIV_ZERO;
                    }
                }
            }
            else if (BinOp == TOKEN_LT) {
                if (executeMode) {
                    stackPushNum(l < r ? 1.0f : 0.0f);
                }
            }
            else if (BinOp == TOKEN_GT) {
                if (executeMode) {
                    stackPushNum(l > r ? 1.0f : 0.0f);
                }
            }
            else if (BinOp == TOKEN_EQUALS) {
                if (executeMode) {
                    stackPushNum(l == r ? 1.0f : 0.0f);
                }
            }
            else if (BinOp == TOKEN_NOT_EQ) {
                if (executeMode) {
                    stackPushNum(l != r ? 1.0f : 0.0f);
                }
            }
            else if (BinOp == TOKEN_LT_EQ) {
                if (executeMode) {
                    stackPushNum(l <= r ? 1.0f : 0.0f);
                }
            }
            else if (BinOp == TOKEN_GT_EQ) {
                if (executeMode) {
                    stackPushNum(l >= r ? 1.0f : 0.0f);
                }
            }
            else if (BinOp == TOKEN_AND) {
                if (executeMode) {
                    stackPushNum(r != 0.0f ? l : 0.0f);
                }
            }
            else if (BinOp == TOKEN_OR) {
                if (executeMode) {
                    stackPushNum(r != 0.0f ? 1 : l);
                }
            }
            else
                return ERROR_UNEXPECTED_TOKEN;
        }
        else if (IS_TYPE_STR(lhsVal) && IS_TYPE_STR(rhsVal)) {   
            /* String operations */
            if (BinOp == TOKEN_PLUS) {
                if (executeMode) {
                    stackAdd2Strs();
                }
            }
            else if (BinOp >= TOKEN_EQUALS && BinOp <=TOKEN_LT_EQ) {
                if (executeMode) {
                    char *r = stackPopStr();
                    char *l = stackPopStr();
                    int ret = strcmp(l,r);
                    if (BinOp == TOKEN_EQUALS && ret == 0) {
                        stackPushNum(1.0f);
                    }
                    else if (BinOp == TOKEN_NOT_EQ && ret != 0) {
                        stackPushNum(1.0f);
                    }
                    else if (BinOp == TOKEN_GT && ret > 0) {
                        stackPushNum(1.0f);
                    }
                    else if (BinOp == TOKEN_LT && ret < 0) {
                        stackPushNum(1.0f);
                    }
                    else if (BinOp == TOKEN_GT_EQ && ret >= 0) {
                        stackPushNum(1.0f);
                    }
                    else if (BinOp == TOKEN_LT_EQ && ret <= 0) {
                        stackPushNum(1.0f);
                    }
                    else {
                        stackPushNum(0.0f);
                    }
                }
                lhsVal = TYPE_NUMBER;
            }
            else {
                return ERROR_UNEXPECTED_TOKEN;
            }
        }
        else {
            return ERROR_UNEXPECTED_TOKEN;
        }
    }
}

int parseExpression() {
    int val = parsePrimary();
    if (val & ERROR_MASK) {
        return val;
    }
    return parseBinOpRHS(0, val);
}

int expectNumber(void) {
    int val = parseExpression();
    if (val & ERROR_MASK) {
        return val;
    }

    if (!IS_TYPE_NUM(val)) {
        return ERROR_EXPR_EXPECTED_NUM;
    }
    return 0;
}

int parse_RUN(void) {
    getNextToken();
    uint16_t startLine = 1;
    if (curToken != TOKEN_EOL) {
        int val = expectNumber();
        if (val) {
            return val;    /* error */
        }

        if (executeMode) {
            startLine = (uint16_t)stackPopNum();
            if (startLine <= 0) {
                return ERROR_BAD_LINE_NUM;
            }
        }
    }

    if (executeMode) {
        /* Clear variables */
        sysVARSTART = sysVAREND = sysGOSUBSTART = sysGOSUBEND = MEMORY_SIZE;
        jumpLineNumber = startLine;
        stopLineNumber = stopStmtNumber = 0;
    }
    return 0;
}

int parse_GOTO(void) {
    getNextToken();
    int val = expectNumber();
    if (val) {
        return val;    /* error */
    }

    if (executeMode) {
        uint16_t startLine = (uint16_t)stackPopNum();
        if (startLine <= 0) {
            return ERROR_BAD_LINE_NUM;
        }
        jumpLineNumber = startLine;
    }
    return 0;
}

int parse_PAUSE(void) {
    getNextToken();
    int val = expectNumber();
    if (val) {
        return val;    /* error */
    }

    if (executeMode) {
        long ms = (long)stackPopNum();
        if (ms < 0) {
            return ERROR_BAD_PARAMETER;
        }
        host_sleep(ms);
    }
    return 0;
}

int parse_LIST(void) {
    getNextToken();
    uint16_t first = 0, last = 0;
    if (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
        int val = expectNumber();
        if (val) {
            return val;    /* error */
        }

        if (executeMode) {
            first = (uint16_t)stackPopNum();
        }
    }

    if (curToken == TOKEN_COMMA) {
        getNextToken();
        int val = expectNumber();
        if (val) {
            return val;    /* error */
        }

        if (executeMode) {
            last = (uint16_t)stackPopNum();
        }
    }

    if (executeMode) {
        listProg(first,last);
        host_showBuffer();
    }
    return 0;
}

int parse_PRINT(void) {
    getNextToken();
    /* zero + expressions seperated by semicolons */
    int newLine = 1;
    while (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
        int val = parseExpression();
        if (val & ERROR_MASK) {
            return val;
        }

        if (executeMode) {
            if (IS_TYPE_NUM(val)) {
                host_outputFloat(stackPopNum());
            }
            else {
                host_outputString(stackPopStr());
            }
            newLine = 1;
        }
        if (curToken == TOKEN_SEMICOLON) {
            newLine = 0;
            getNextToken();
        }
    }

    if (executeMode) {
        if (newLine) {
            host_newLine();
        }
        host_showBuffer();
    }
    return 0;
}

/* Parse a stmt that takes two int parameters 
   e.g. POSITION 3,2 */
int parseTwoIntCmd(void) {
    int op = curToken;
    getNextToken();
    int val = expectNumber();
    if (val) {
        return val;    /* error */
    }

    if (curToken != TOKEN_COMMA) {
        return ERROR_UNEXPECTED_TOKEN;
    }

    getNextToken();
    val = expectNumber();
    if (val) {
        return val;    /* error */
    }

    if (executeMode) {
        int second = (int)stackPopNum();
        int first = (int)stackPopNum();
        switch(op) {
        case TOKEN_POSITION: 
            host_moveCursor(first,second); 
            break;
        case TOKEN_PIN: 
            host_digitalWrite(first,second); 
            break;
        case TOKEN_PINMODE: 
            host_pinMode(first,second); 
            break;
        }
    }
    return 0;
}

/* This handles both LET a$="hello" and INPUT a$ type assignments */
int parseAssignment(bool inputStmt) {
    char ident[MAX_IDENT_LEN+1];
    int val;
    if (curToken != TOKEN_IDENT) {
        return ERROR_UNEXPECTED_TOKEN;
    }

    if (executeMode) {
        strcpy(ident, identVal);
    }

    int isStringIdentifier = isStrIdent;
    int isArray = 0;
    getNextToken(); /* eat ident */
    if (curToken == TOKEN_LBRACKET) {
        /* array element being set */
        val = parseSubscriptExpr();
        if (val) {
            return val;
        }

        isArray = 1;
    }

    if (inputStmt) {
        /* From INPUT statement */
        if (executeMode) {
            char *inputStr = host_readLine();
            if (isStringIdentifier) {
                if (!stackPushStr(inputStr)) {
                    return ERROR_OUT_OF_MEMORY;
                }
            }
            else {
                float f = (float)strtod(inputStr, 0);
                if (!stackPushNum(f)) {
                    return ERROR_OUT_OF_MEMORY;
                }
            }
            host_newLine();
            host_showBuffer();
        }
        val = isStringIdentifier ? TYPE_STRING : TYPE_NUMBER;
    }
    else {
        /* From LET statement */
        if (curToken != TOKEN_EQUALS) {
            return ERROR_UNEXPECTED_TOKEN;
        }

        getNextToken(); /* eat = */
        val = parseExpression();
        if (val & ERROR_MASK) {
            return val;
        }
    }

    /* Type checking and actual assignment */
    if (!isStringIdentifier) {   
        /* Numeric variable */
        if (!IS_TYPE_NUM(val)) {
            return ERROR_EXPR_EXPECTED_NUM;
        }

        if (executeMode) {
            if (isArray) {
                val = setNumArrayElem(ident, stackPopNum());
                if (val) {
                    return val;
                }
            }
            else {
                if (!storeNumVariable(ident, stackPopNum())) {
                    return ERROR_OUT_OF_MEMORY;
                }
            }
        }
    }
    else {   
        /* String  */
        if (!IS_TYPE_STR(val)) {
            return ERROR_EXPR_EXPECTED_STR;
        }

        if (executeMode) {
            if (isArray) {
                /* Annoyingly, we've got the string at the top of the stack
                   (from parseExpression) and the array index stuff (from
                   parseSubscriptExpr) underneath */
                val = setStrArrayElem(ident);
                if (val) {
                    return val;
                }
            }
            else {
                if (!storeStrVariable(ident, stackGetStr())) {
                    return ERROR_OUT_OF_MEMORY;
                }
                stackPopStr();
            }
        }
    }
    return 0;
}

int parse_IF(void) {
    getNextToken(); /* eat if */
    int val = expectNumber();
    if (val) {
        return val;    /* error */
    }

    if (curToken != TOKEN_THEN) {
        return ERROR_MISSING_THEN;
    }
    getNextToken();
    if (executeMode && stackPopNum() == 0.0f) {
        /* Condition not met */
        breakCurrentLine = 1;
        return 0;
    }
    else {
        return 0;
    }
}

int parse_FOR(void) {
    char ident[MAX_IDENT_LEN+1];
    float start = 0;
    float end = 0;
    float step = 1.0f;
    getNextToken(); /* eat for */
    if (curToken != TOKEN_IDENT || isStrIdent) {
        return ERROR_UNEXPECTED_TOKEN;
    }

    if (executeMode) {
        strcpy(ident, identVal);
    }
    getNextToken(); /* eat ident */
    if (curToken != TOKEN_EQUALS) {
        return ERROR_UNEXPECTED_TOKEN;
    }
    getNextToken(); /* eat = */

    /* Parse START */
    int val = expectNumber();
    if (val) {
        return val;    /* error */
    }

    if (executeMode) {
        start = stackPopNum();
    }

    /* Parse TO */
    if (curToken != TOKEN_TO) {
        return ERROR_UNEXPECTED_TOKEN;
    }
    getNextToken(); /* eat TO */

    /* Parse END */
    val = expectNumber();
    if (val) {
        return val;    /* error */
    }

    if (executeMode) {
        end = stackPopNum();
    }

    /* Parse optional STEP */
    if (curToken == TOKEN_STEP) {
        getNextToken(); /* eat STEP */
        val = expectNumber();
        if (val) {
            return val;    /* error */
        }

        if (executeMode) {
            step = stackPopNum();
        }
    }

    if (executeMode) {
        if (!storeForNextVariable(ident, start, step, end, lineNumber, stmtNumber)) {
            return ERROR_OUT_OF_MEMORY;
        }
    }
    return 0;
}

int parse_NEXT(void) {
    getNextToken(); /* eat next */
    if (curToken != TOKEN_IDENT || isStrIdent) {
        return ERROR_UNEXPECTED_TOKEN;
    }

    if (executeMode) {
        ForNextData data = lookupForNextVariable(identVal);
        if (data.val == FLT_MAX) {
            return ERROR_VARIABLE_NOT_FOUND;
        }
        else if (data.step == FLT_MAX) {
            return ERROR_NEXT_WITHOUT_FOR;
        }

        /* Update and store the count variable */
        data.val += data.step;
        storeNumVariable(identVal, data.val);

        /* loop? */
        if ((data.step >= 0 && data.val <= data.end) || (data.step < 0 && data.val >= data.end)) {
            jumpLineNumber = data.lineNumber;
            jumpStmtNumber = data.stmtNumber + 1;
        }
    }
    getNextToken(); /* eat ident */
    return 0;
}

int parse_GOSUB(void) {
    getNextToken(); /* eat gosub */
    int val = expectNumber();
    if (val) {
        return val;    /* error */
    }

    if (executeMode) {
        uint16_t startLine = (uint16_t)stackPopNum();
        if (startLine <= 0) {
            return ERROR_BAD_LINE_NUM;
        }

        jumpLineNumber = startLine;
        if (!gosubStackPush(lineNumber,stmtNumber)) {
            return ERROR_OUT_OF_MEMORY;
        }
    }
    return 0;
}

/* LOAD or LOAD "x"
   SAVE, SAVE+ or SAVE "x"
   DELETE "x" */
int parseLoadSaveCmd(void) {
#ifndef SD_CARD_IN_USE
    return ERROR_BAD_PARAMETER;
#endif
    int op = curToken;
    /* TODO char autoexec = 0; */

    char gotFileName = 0;

    getNextToken();
    if (op == TOKEN_SAVE && curToken == TOKEN_PLUS) {
        getNextToken();
        /* TODO autoexec = 1; */
    }
    else if (curToken != TOKEN_EOL && curToken != TOKEN_CMD_SEP) {
        int val = parseExpression();
        if (val & ERROR_MASK) {
            return val;
        }

        if (!IS_TYPE_STR(val)) {
            return ERROR_EXPR_EXPECTED_STR;
        }
        gotFileName = 1;
    }

    if (executeMode) {
        if (gotFileName) {
#ifdef SD_CARD_IN_USE
            char fileName[MAX_IDENT_LEN + 1];

            if (strlen(stackGetStr()) > MAX_IDENT_LEN) {
                return ERROR_BAD_PARAMETER;
            }

            strcpy(fileName, stackPopStr());

            if (op == TOKEN_SAVE) {
                if (!host_saveSdCard(fileName)) {
                    return ERROR_OUT_OF_MEMORY;
                }
            }
            else if (op == TOKEN_LOAD) {
                reset();
                if (!host_loadSdCard(fileName)) {
                    return ERROR_BAD_PARAMETER;
                }
            }
            else if (op == TOKEN_DELETE) {
 /*               if (!host_removeExtEEPROM(fileName))
                      return ERROR_BAD_PARAMETER; */
            }
#endif
        }
        else {
            if (op == TOKEN_SAVE) {
                /* TODO host_saveProgram(autoexec); */
                return ERROR_BAD_PARAMETER;
            }
            else if (op == TOKEN_LOAD) {
              /* TODO  reset();
                host_loadProgram(); */
               return ERROR_BAD_PARAMETER; 
            }
            else {
                return ERROR_UNEXPECTED_CMD;
            }
        }
    }
    return 0;
}

int parseSimpleCmd(void) {
    int op = curToken;
    getNextToken(); /* eat op */
    if (executeMode) {
        switch (op) {
            case TOKEN_NEW:
                reset();
                breakCurrentLine = 1;
                break;
            case TOKEN_STOP:
                stopLineNumber = lineNumber;
                stopStmtNumber = stmtNumber;
                return ERROR_STOP_STATEMENT;
            case TOKEN_CONT:
                if (stopLineNumber) {
                    jumpLineNumber = stopLineNumber;
                    jumpStmtNumber = stopStmtNumber+1;
                }
                break;
            case TOKEN_RETURN:
            {
                int returnLineNumber, returnStmtNumber;
                if (!gosubStackPop(&returnLineNumber, &returnStmtNumber)) {
                    return ERROR_RETURN_WITHOUT_GOSUB;
                }
                jumpLineNumber = returnLineNumber;
                jumpStmtNumber = returnStmtNumber+1;
                break;
            }
            case TOKEN_CLS:
                host_cls();
                host_showBuffer();
                break;
            case TOKEN_DIR:
#ifdef EXTERNAL_EEPROM /* TODO */
                host_directoryExtEEPROM();
#endif
                break;
        }
    }
    return 0;
}

int parse_DIM(void) {
    char ident[MAX_IDENT_LEN + 1];
    getNextToken(); /* eat DIM */
    if (curToken != TOKEN_IDENT) {
        return ERROR_UNEXPECTED_TOKEN;
    }

    if (executeMode) {
        strcpy(ident, identVal);
    }
    int isStringIdentifier = isStrIdent;
    getNextToken(); /* eat ident */
    int val = parseSubscriptExpr();
    if (val) {
        return val;
    }

    if (executeMode && !createArray(ident, isStringIdentifier ? 1 : 0)) {
        return ERROR_OUT_OF_MEMORY;
    }
    return 0;
}

static int targetStmtNumber;
int parseStmts(void) {
    int ret = 0;
    breakCurrentLine = 0;
    jumpLineNumber = 0;
    jumpStmtNumber = 0;

    while (ret == 0) {
        if (curToken == TOKEN_EOL) {
            break;
        }

        if (executeMode) {
            sysSTACKEND = sysSTACKSTART = sysPROGEND;   /* Clear calculator stack */
        }

        int needCmdSep = 1;
        switch (curToken) {
        case TOKEN_PRINT: ret = parse_PRINT(); break;
        case TOKEN_LET: getNextToken(); ret = parseAssignment(false); break;
        case TOKEN_IDENT: ret = parseAssignment(false); break;
        case TOKEN_INPUT: getNextToken(); ret = parseAssignment(true); break;
        case TOKEN_LIST: ret = parse_LIST(); break;
        case TOKEN_RUN: ret = parse_RUN(); break;
        case TOKEN_GOTO: ret = parse_GOTO(); break;
        case TOKEN_REM: getNextToken(); getNextToken(); break;
        case TOKEN_IF: ret = parse_IF(); needCmdSep = 0; break;
        case TOKEN_FOR: ret = parse_FOR(); break;
        case TOKEN_NEXT: ret = parse_NEXT(); break;
        case TOKEN_GOSUB: ret = parse_GOSUB(); break;
        case TOKEN_DIM: ret = parse_DIM(); break;
        case TOKEN_PAUSE: ret = parse_PAUSE(); break;
        
        case TOKEN_LOAD:
        case TOKEN_SAVE:
        case TOKEN_DELETE:
            ret = parseLoadSaveCmd();
            break;
        
        case TOKEN_POSITION:
        case TOKEN_PIN:
        case TOKEN_PINMODE:
            ret = parseTwoIntCmd(); 
            break;
            
        case TOKEN_NEW:
        case TOKEN_STOP:
        case TOKEN_CONT:
        case TOKEN_RETURN:
        case TOKEN_CLS:
        case TOKEN_DIR:
            ret = parseSimpleCmd();
            break;
            
        default: 
            ret = ERROR_UNEXPECTED_CMD;
        }

        /* If error, or the execution line has been changed, exit here */
        if (ret || breakCurrentLine || jumpLineNumber || jumpStmtNumber) {
            break;
        }

        /* it should either be the end of the line now, and (generally) a command seperator
           before the next command */
        if (curToken != TOKEN_EOL) {
            if (needCmdSep) {
                if (curToken != TOKEN_CMD_SEP) {
                    ret = ERROR_UNEXPECTED_CMD;
                }
                else {
                    getNextToken();

                    /* Don't allow a trailing : */
                    if (curToken == TOKEN_EOL) {
                        ret = ERROR_UNEXPECTED_CMD;
                    }
                }
            }
        }

        /* Increase the statement number */
        stmtNumber++;
        
        /* If we're just scanning to find a target statement, check */
        if (stmtNumber == targetStmtNumber) {
            break;
        }
    }
    return ret;
}

int processInput(unsigned char *tokenBuf) {
    /* First token can be TOKEN_INTEGER for line number - stored as a float in numVal
       store as WORD line number (max 65535) */
    tokenBuffer = tokenBuf;
    getNextToken();
    
    /* Check for a line number at the start */
    uint16_t gotLineNumber = 0;
    unsigned char *lineStartPtr = 0;
    if (curToken == TOKEN_INTEGER) {
        long val = (long)numVal;
        if (val <= 65535) {
            gotLineNumber = (uint16_t)val;
            lineStartPtr = tokenBuffer;
            getNextToken();
        }
        else {
            return ERROR_LINE_NUM_TOO_BIG;
        }
    }

    executeMode = 0;
    targetStmtNumber = 0;
    int ret = parseStmts(); /* Syntax check */
    if (ret != ERROR_NONE) {
        return ret;
    }

    if (gotLineNumber) {
        if (!doProgLine(gotLineNumber, lineStartPtr, tokenBuffer - lineStartPtr)) {
            ret = ERROR_OUT_OF_MEMORY;
        }
    }
    else {
        /* We start off executing from the input buffer */
        tokenBuffer = tokenBuf;
        executeMode = 1;
        lineNumber = 0; /* Buffer */
        unsigned char *p = NULL;

        while (1) {
            getNextToken();

            stmtNumber = 0;

            /* Skip any statements? (e.g. for/next) */
            if (targetStmtNumber) {
                executeMode = 0; 
                parseStmts(); 
                executeMode = 1;
                targetStmtNumber = 0;
            }

            /* Now execute */
            ret = parseStmts();
            if (ret != ERROR_NONE) {
                break;
            }

            /* Are we processing the input buffer? */
            if (!lineNumber && !jumpLineNumber && !jumpStmtNumber) {
                break;  /* If no control flow jumps, then just exit */
            }

            /* Are we RETURNing to the input buffer? */
            if (lineNumber && !jumpLineNumber && jumpStmtNumber) {
                lineNumber = 0;
            }

            if (!lineNumber && !jumpLineNumber && jumpStmtNumber) {
                /* We're executing the buffer, and need to jump stmt (e.g. for/next) */
                tokenBuffer = tokenBuf;
            }
            else {
                /* We're executing the program */
                if (jumpLineNumber || jumpStmtNumber) {
                    /* Line/statement number was changed e.g. goto */
                    p = findProgLine(jumpLineNumber);
                }
                else {
                    /* Line number didn't change, so just move one to the next one */
                    p += *(uint16_t *)p;
                }

                /* End of program? */
                if (p == &mem[sysPROGEND]) {
                    break;  /* End of program */
                }

                lineNumber = *(uint16_t*)(p + 2);
                tokenBuffer = p + 4;

                /* If the target for a jump is missing (e.g. line deleted) and we're on the next line
                   reset the stmt number to 0 */
                if (jumpLineNumber && jumpStmtNumber && lineNumber > jumpLineNumber) {
                    jumpStmtNumber = 0;
                }
            }

            if (jumpStmtNumber) {
                targetStmtNumber = jumpStmtNumber;
            }

            if (host_ESCPressed())
            { 
                ret = ERROR_BREAK_PRESSED; 
                break; 
            }
        }
    }
    return ret;
}

void reset() {
    /* Program at the start of memory */
    sysPROGEND = 0;

    /* Stack is at the end of the program area */
    sysSTACKSTART = sysSTACKEND = sysPROGEND;

    /* Variables/gosub stack at the end of memory */
    sysVARSTART = sysVAREND = sysGOSUBSTART = sysGOSUBEND = MEMORY_SIZE;
    memset(&mem[0], 0, MEMORY_SIZE);

    stopLineNumber = 0;
    stopStmtNumber = 0;
    lineNumber = 0;
}
