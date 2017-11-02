/*
Dana Olcott
10/31/17

Button LED init:
Program that starts other programs, including:

lkm/button3
devices/pin26
devices/pin27

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>             //"system" function

int main( int argc, char *argv[] )
{
	//buttons with interrupts
    system("echo INSMOD: lkm button3");
    system("insmod /home/debian/lkm/button3/button3.ko");
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

