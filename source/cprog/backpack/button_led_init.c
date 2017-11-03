/*
Dana Olcott
10/31/17

Button LED init:
Program that starts other programs, including:

lkm/button2
devices/pin26
devices/pin27

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>             //"system" function

int main( int argc, char *argv[] )
{
	//buttons with interrupts - center and right
    system("echo INSMOD: lkm button2");
    system("insmod /home/debian/lkm/button2/button2.ko");
    system("sleep 1s");

    //configure the left button as input, falling edge
    system("echo Configure buttonLeft");
    system("echo 112 > /sys/class/gpio/export");
    system("echo in > /sys/class/gpio/gpio112/direction");
    system("echo falling > /sys/class/gpio/gpio112/edge");
    system("sleep 1s");

    //led driver pin26 - blue led
    system("echo INSMOD: devices pin26");
    system("insmod /home/debian/devices/pin26/pin26.ko");
    system("sleep 1s");

    //led driver pin27 - red led
    system("echo INSMOD: devices pin27");
    system("insmod /home/debian/devices/pin27/pin27.ko");
    system("sleep 1s");

    return 0;
}

