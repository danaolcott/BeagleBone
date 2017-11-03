/*
Dana Olcott
10/31/17

Button LED deinit:
Program that stops other programs, including:

lkm/button2
devices/pin26
devices/pin27

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>             //"system" function

int main( int argc, char *argv[] )
{
    system("echo RMMOD: lkm button2");
    system("rmmod button2");
    system("sleep 1s");

    system("echo RMMOD: pin26");
    system("rmmod pin26");
    system("sleep 1s");

    system("echo RMMOD: pin27");
    system("rmmod pin27");
    system("sleep 1s");

    return 0;
}

