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
    system("/home/debian/cprogs/backpack/rtos_backpack");

    return 0;
}

