/*
 * main.c
 *
 *  Created on: Sep 25, 2017
 *      Author: danao
 *
 *      Simple Blink Project
 *      Toggle GPIO Pin 9-12 on the BeagleBoard
 *      using /dev/pin9_12 device driver.
 *      Install the driver prior to running the program
 *      using "insmod /home/debian/devices/pin9_12.ko"
 *
 *      Configure timer as Real or Virtual.  Real
 *      timer accounts for time that the process
 *      is running and not running.  Virtual timer
 *      only accounts for the time that the process
 *      is running.  ie, a real timer will timeout
 *      faster than a virtual timer.
 *
 */

#include <stdio.h>

#include "main.h"

int main()
{
	led_init();						//init the user leds
	gpio_init();					//init gpio pins
	timer_init(TIMER_TYPE_REAL);	//init timer / timebase

	while (1)
	{
		printf("Pin9_12 OFF!!\n");

		gpio_set(GPIO_PIN_9_12, GPIO_STATE_OFF);
		printf("Readback Value: %d\n", gpio_get_state(GPIO_PIN_9_12));
		timer_delay(1000);

		printf("Pin9_12 ON!!\n");
		gpio_set(GPIO_PIN_9_12, GPIO_STATE_ON);
		printf("Readback Value: %d\n", gpio_get_state(GPIO_PIN_9_12));
		timer_delay(1000);
	}
	return 0;
}





