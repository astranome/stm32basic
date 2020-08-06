BINARY = sd_test

OBJS = ../src/utility.o
OBJS += ../src/ps2kbd.o
OBJS += ../src/lcd.o
OBJS += ../src/comm.o
OBJS += ../src/term_io.o
OBJS += ../src/rtc.o
OBJS += ../src/fattime.o
OBJS += ../chan_fatfs/src/ff.o
OBJS += ../src/sd_spi.o

# Configuration flags
DEFS += -DSERIAL_TRACES_ENABLED
#DEFS += -DEXTRA_TRACES_ENABLED
DEFS += -DSD_CARD_IN_USE

# Uncomment one of keyboard's layout to be used
DEFS += -DKEYBOARD_US
#DEFS += -DKEYBOARD_GERMAN
#DEFS += -DKEYBOARD_FRENCH
#DEFS += -DKEYBOARD_SPANISH
#DEFS += -DKEYBOARD_ITALIAN
#DEFS += -DKEYBOARD_UK

OPENCM3_DIR=../libopencm3
LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f1/stm32f103x8.ld

include ../libopencm3.target.mk
