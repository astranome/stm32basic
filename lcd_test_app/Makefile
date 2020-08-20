BINARY = lcd_test

OBJS = ../src/utility.o
OBJS += ../src/lcd2004.o
OBJS += ../src/comm.o
OBJS += ../src/term_io.o
OBJS += ../src/rtc.o
OBJS += ../src/fattime.o

# Configuration flags
DEFS += -DSERIAL_TRACES_ENABLED
#DEFS += -DSD_CARD_IN_USE

OPENCM3_DIR=../libopencm3
LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f1/stm32f103x8.ld

include ../libopencm3.target.mk
