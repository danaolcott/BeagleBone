/*
Dana Olcott
10/31/17

LCD deinit:
Program that stops other programs, including:

sysfs_lcd - rmmod
lcd_interface - rmmod

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>             //"system" function

int main( int argc, char *argv[] )
{
    //sysfs - has to be removed first since it depends on spi_interface
    system("echo Removing: sysfs_lcd");
    system("rmmod sysfs_lcd");
    system("sleep 1s");

    //lcd_interface
    system("echo Removing: lcd_interface");
    system("rmmod lcd_interface");
    system("sleep 1s");

    return 0;
}

