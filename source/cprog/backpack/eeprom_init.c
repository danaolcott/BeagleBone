/*
Dana Olcott
10/31/17

EEPROM init:
Program that starts other programs, including:

spi_config - run
spi_interface - insmod
sysfs_eeprom - insmod

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>             //"system" function

int main( int argc, char *argv[] )
{
    //spi_config
    system("echo RUN: spi_config");
    system("/home/debian/lkm/spi_interface/spi_config 0 8 1000000");
    system("sleep 1s");

    system("echo INSMOD: spi_interface");
    system("insmod /home/debian/lkm/spi_interface/spi_interface.ko");
    system("sleep 1s");

    system("echo INSMOD: sysfs_eeprom");
    system("insmod /home/debian/sysfs/eeprom/sysfs_eeprom.ko");
    system("sleep 1s");

    return 0;
}

