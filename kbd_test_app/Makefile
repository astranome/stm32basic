BINARY = kbd_test

OBJS = ../src/utility.o
OBJS += ../src/ps2kbd.o
OBJS += ../src/lcd2004.o
OBJS += ../src/comm.o
OBJS += ../src/term_io.o

# Configuration flags
DEFS += -DSERIAL_TRACES_ENABLED
#DEFS += -DVERBOSE_TRACES
#DEFS += -DSD_CARD_IN_USE

# Uncomment one of keyboard's layout to be used
DEFS += -DKEYBOARD_US
#DEFS += -DKEYBOARD_GERMAN
#DEFS += -DKEYBOARD_FRENCH
#DEFS += -DKEYBOARD_SPANISH
#DEFS += -DKEYBOARD_ITALIAN
#DEFS += -DKEYBOARD_UK

OPENCM3_DIR=../libopencm3
LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f1/stm32f103xe.ld

include ../libopencm3.target.mk
