# BeagleBoneBlack
This repository contains projects that explore kernel module and user space programming concepts.  See information below as well as Wiki page(s) for more information.  Photo below shows BeagleBoneBlack Rev C with Backpack PCA.  Note bad soldering of throughhole parts.  I put the resistors in the wrong location and tried to remove them, it didn't work out too well.  The traces are widely space as are the components.  Lots more room on the board, especially if switch to surface mount parts.  The LCD is a 4bit LCD from EA Displays.  The display was selected so that the driver could be scratch built using a handfull of several GPIO pins.  The basic demo includes setting a write address and writing a value to EEPROM, reading the value back, and writing the readback value to the LCD.  Red and blue LEDs flash when user buttons are pressed. 

![alt text](https://raw.githubusercontent.com/danaolcott/BeagleBone/master/source/photos/beagleboneblack_demo2.jpg)


Hardware
--------
- BeagleBoneBlack Rev C
- "Backpack" module that stacks on headers P8 and P9 (align one row back, starting at pins 3 and 4 (1 and 2 not connected).


Software
--------
Debian Linux distribution XXXXXX

User space programs developed on Windows using Eclipse and the Windows/BeagleBone Cross-Toolchain.  The toolchain can be found at:



Backpack
-----------
Backpack contains the following devices:
- 2 User LEDs on GPIO26 and GPIO27
- 3 User Buttons on GPIO112, 115, and 61 (Note: Schematics show the right button is on GPIO117, but as it turned out, GPIO117 is configured for something related to a clock, so it was changed to GPIO61.  This required a jumper and cutting one trace.
- SPI EEPROM IC connected to SPI0.
- LCD with 4bit interface
- Vias also added for UART4, I2C, two PWM, and two GPIO (not including GPIO 61).

Backpack built as a 2-layer board from ExpressPCB with no solder mask or silkscreen.  This was the cheapest option I found and worked well.  The boards arrived in 2 days.  Schematics and layout files located in source/backpack.
