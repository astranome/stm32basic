BINARY = tft320240_test

OBJS = ../src/utility.o
OBJS += ../src/tft320240.o
OBJS += ../src/comm.o
OBJS += ../src/term_io.o
OBJS += ../src/rtc.o
OBJS += ../src/fattime.o

# Configuration flags
DEFS += -DSERIAL_TRACES_ENABLED
DEFS += -DDSO138_PLATFORM

OPENCM3_DIR=../libopencm3
LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f1/stm32f103x8.ld

include ../libopencm3.target.mk
