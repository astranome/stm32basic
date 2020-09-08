BINARY = tftspi_test

OBJS = ../src/utility.o
#OBJS += ../src/lcd2004.o
OBJS += ../src/comm.o
OBJS += ../src/term_io.o
OBJS += ../src/rtc.o
OBJS += ../src/fattime.o

# Configuration flags
DEFS += -DSERIAL_TRACES_ENABLED

OPENCM3_DIR=../libopencm3
LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f1/stm32f103xe.ld

include ../libopencm3.target.mk
