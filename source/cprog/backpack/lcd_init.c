/*
Dana Olcott
10/31/17

LCD init:
Program that starts other programs, including:

lcd_interface - insmod
sysfs_lcd - insmod
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>             //"system" function

int main( int argc, char *argv[] )
{
    system("echo INSMOD: lcd_interface");
    system("insmod /home/debian/lkm/lcd_interface/lcd_interface.ko");
    system("sleep 1s");

    system("echo INSMOD: sysfs_lcd");
    system("insmod /home/debian/sysfs/lcd/sysfs_lcd.ko");
    system("sleep 1s");

    return 0;
}

