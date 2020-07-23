# STM32Basic

![](docs/images/logo.PNG)

## Retro computer with BASIC interpreter (GCC/libopencm3 toolchain version)
_Based on https://github.com/robinhedwards/ArduinoBASIC Arduino-BASIC_

* MCU: STM32F103C8T6 (72 MHz, 20 KB RAM, 64 KB Flash), a.k.a. "bluepill"
* PS2 keyboard 
* 20x4 LCD Alphanumeric Display with I2C interface
* SD card

## Prerequisites
1: STM32F103C8T6 (72 MHz, 20 KB RAM, 64 KB Flash), a.k.a. "bluepill" board. It should also work on more powerfull devices, e.g. STM32F103RET6.

2: A PS/2 Keyboard. See http://playground.arduino.cc/Main/PS2Keyboard for wiring details.

3: Any of standard HD44780 LCDs 20 x 4 symbols.

4: Breadboard + wires.

5: ST-Link USB adapter.

6: USB-to-COM adapter.

7: (Optional) A Piezoelectric buzzer for keyboard clicks and other sounds.

8: (Optional) SD card slot for breadboards.

## Getting Started

Assuming that the user have Windows based PC, the easiest way to build the firmware and flash it in to MCU is as following:

1: Install [STM32 ST-LINK utility](https://www.st.com/en/development-tools/stsw-link004.html) on Windows PC.

2: Connect ST-Link adapter to stm32 board and verify that the MCU is visible from there.

3: Use any of Ubuntu based hosts machines for making a firmware. E.g. use Windows Subsystem for Linux (WSL) on Windows 10 or Raspberry Pi as a standalone build machine.

### Setting up Linux environment and build applications
* Get ARM Toolchain:

`sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi gdb-arm-none-eabi`

* Clone this Stm32Basic repository and get libopencm3 library:

`cd Stm32Basic`

`./get_libopencm3.sh`

* Build keyboard hardware test

`cd kbd_test_app`

`make hex V=1`

* Build LCD hardware test

`cd ..` 
`cd lcd_test_app`

`make hex V=1`

* Build LCD hardware test

`cd ..` 
`cd lcd_test_app`

`make hex V=1`

* Build SD hardware test

`cd ..` 
`cd sd_test_app`

`make hex V=1`

* Build Stm32Basic application

`cd ..` 
`cd stmbasic_app`

`make hex V=1`

### Get the hardware sorted out

_TODO_

### Write firmware to the board

If the compilation was successful, then one should have `.hex` binary files available, under each of application folder. Copy them to your Windows machine and use ST-Link utility to program and verify the device.

### Backlog
* [x] Get SD card working (for saving and loading .BAS files)
* [ ] Add vertical scrolling
* [ ] Add support to write and read GPIO from BASIC
* [ ] Add support for graphics LCD (e.g. 128 x 64 pixels)






