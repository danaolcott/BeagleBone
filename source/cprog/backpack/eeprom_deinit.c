/*
Dana Olcott
10/31/17

EEPROM deinit:
Program that stops other programs, including:

sysfs_eeprom - rmmod
spi_interface - rmmod

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>             //"system" function

int main( int argc, char *argv[] )
{
    //sysfs - has to be removed first since it depends on spi_interface
    system("echo Removing: sysfs_eeprom");
    system("rmmod sysfs_eeprom");
    system("sleep 1s");

    //spi_config
    system("echo Removing: spi_interface");
    system("rmmod spi_interface");
    system("sleep 1s");

    return 0;
}

