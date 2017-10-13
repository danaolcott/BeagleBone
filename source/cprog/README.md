# bbb/source/cprog
BeagleBoneBlack Projects - C Programs

Programs that run in user space developed for the BeagleBoneBlack.  Programs are developed in Eclipse using the Windows toolchain for the BeagleBoneBlack.  See this link:  http://gnutoolchains.com/beaglebone/.  Download the toolchain version used: http://sysprogs.com/files/gnutoolchains/beaglebone/beaglebone-gcc4.9.2.exe.  

Note:
Some of the programs require a module to be copied, compiled, and insmod before the program will work.  These are located in /source/lkm.  The intent for some of these programs was to write a module followed by a c program that would use the module (for example /cprogs/bbb_blink2 using GPIO P9-12).

How To Run
----------

- Download and install eclipse and the Windows toolchain.  
- Import project and compile.
- ssh into BeagleBoneBlack and copy the target file into folder of users choice.
- -> "chmod +x myFile"
- -> "./myFile


bbb_blink1
----------
A program that toggles user leds on a timer using the linux interval timer

bbb_blink2
----------
A program that toggles GPIO P9-12.  Uses pin9_12 device driver file located in /source/devices/pin9_12.  This module needs to be installed for the program to work.

bbb_rtos_2
----------
FreeRTOS/Posix running on the BeagleBoneBlack.  The project contains a handfull of driver files that use the adc, gpio, spi, access to an eeprom IC (Microchip)... and a handful of others.  The project started out as a stripped down version of the official FreeRTOS Linux port.  It changes from time to time when using a task to test something else in the repo.


