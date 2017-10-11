//////////////////////////////////////////////////
/* 10/10/17
 * Dana Olcott
 * button_driver.h
 * Header file for button driver using sysfs

 Button controlls a switch that is connected to
 and led.  Press the button to toggle the switch
 There are 2 attributes assigned to the button 
 kobject: presses, state
 you can read or write to either one of them.

 */
#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <stdint.h>


//sysfs file paths for button kobject attributes
#define BUTTON_PRESSES_PATH		((char*)"/sys/kernel/button/presses");
#define BUTTON_STATE_PATH		((char*)"/sys/kernel/button/state");


void button_init(void);

//button
uint16_t button_getNumPresses(void);
void button_setNumPresses(uint16_t num);

//switch state - led conntected to the button
uint16_t button_getState(void);
void button_setState(uint16_t state);

#endif
