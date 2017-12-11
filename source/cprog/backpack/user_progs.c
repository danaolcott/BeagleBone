/*
Dana Olcott
10/31/17

user_progs
start all the user programs we want to run

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>             //"system" function

int main( int argc, char *argv[] )
{
    system("sleep 1s");
    system("/home/debian/cprogs/backpack/rtos_backpack &");
    system("sleep 1s");

    system("echo none > /sys/class/leds/beaglebone:green:usr0/trigger");
    system("echo none > /sys/class/leds/beaglebone:green:usr1/trigger");
    system("echo none > /sys/class/leds/beaglebone:green:usr2/trigger");
    system("echo none > /sys/class/leds/beaglebone:green:usr3/trigger");

    system("sleep 1s");
    system("/home/debian/cprogs/backpack/task_backpack &");

  

    return 0;
}

