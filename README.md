# BeagleBoneBlack
This repository contains projects that explore kernel module and user space programming concepts.  See information below as well as Wiki page(s) for more information.  Photo below shows BeagleBoneBlack Rev C with Backpack PCA.  

![alt text](https://raw.githubusercontent.com/danaolcott/BeagleBone/master/source/photos/beagleboneblack_demo2.jpg)


Hardware
--------
- BeagleBoneBlack Rev C
- "Backpack" module that stacks on headers P8 and P9 (align one row back, starting at pins 3 and 4 (1 and 2 not connected).  Backpack module contains a 3 line x 16 character display, user buttons, LEDs, and vias connected to UART, I2C, PWM, and a few extra GPIOs.  The PCB was made using ExpressPCB, and arrived within a few days of ordering.  Some errors in the board include vias that are undersized(trouble installing throughhole parts), and right user button mapped to GPIO 117, which is used by the BeagleBone for external osc? pin.  I had to cut one trace and run a wire to GPIO61.  The trace spacing is pretty large and components could be smaller.  There's always a revsion 2....

Software
--------
Debian Linux distribution XXXXXX

User space programs developed on Windows using Eclipse and the Windows/BeagleBone Cross-Toolchain.  The toolchain can be found at:http://gnutoolchains.com/

Building kernel modules requires downloading and installing the kernel header files.  You don't need the entire kernel source, just the headers.  Here are a few good links to help:
https://elinux.org/Beagleboard:BeagleBoneBlack_Debian

My favorite source of information so far:
http://derekmolloy.ie/writing-a-linux-kernel-module-part-1-introduction/


Backpack Pinout and Components
------------------------------
Backpack contains the following devices:
- 2 User LEDs on GPIO26 and GPIO27
- 3 User Buttons on GPIO112, 115, and 61 (Note: Schematics show the right button is on GPIO117, but as it turned out, GPIO117 is configured for something related to a clock, so it was changed to GPIO61.  This required a jumper and cutting one trace.
- SPI EEPROM IC from Microchip (PN# 25AA640A) connected to SPI0 (GPIO Pins P9.17, P9.18, P9.21, and P9.22).
- LCD from Electronic Assembly PN# EADOGM163x-A (Digikey PN#: 1481-1081-ND).  The LCD has a 4bit interface and was selected so the driver could be scratch built using a handful of GPIO pins.
- Vias also added for UART4 (P9.11 and P9.13), I2C (P9.19 and P9.20), two PWM, and two GPIO (P9.12, and..) (not including GPIO 61).


Backpack Demonstration Software
-------------------------------
To demonstrate the features of the backpack board, build, install, and include the module backpack.ko (source/lkm/backpack) in the boot up routine (/etc/modules?).  The module uses a workqueue to create a 10 second delay, followed by posting upcalls (terminology?, call_usermodehelper) to run user space programs for configuring SPI0 and installing subsequent modules.  The modules make use of the following concepts:
- EXPORT - spi_interface and lcd_interface
- /dev/ - GPIO Pins 26, 27 for flashing LEDs
- /sys/kernel/lcd - sysfs to generate files / attributes to read and write to the lcd.
- /sys/kernel/eeprom - sysfs to generate files / attributes to read and write to the user eeprom.
- Button interrupts initialized in the kernel module (user buttons center and right) as well as in user space (left button)
- User space Linux port for FreeRTOS (source/cprogs/rtos_backpack).  The program contains several tasks, each controlling a different part of the system.  For example, a timed task for flashing user leds, event driven task to receive messags to draw text on the display, and a memory task for reading and writing values to eeprom.

The demo program does not really do anything of any real significance.  The purpose of the program is to demonstrate what I've learned so far about embedded linux programming.  


