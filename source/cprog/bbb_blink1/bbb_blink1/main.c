/*
 * main.c
 *
 *  Created on: Sep 25, 2017
 *      Author: danao
 *
 *      Simple Hello Project
 *      Print hello to the terminal on a timer.

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

	timer_init(TIMER_TYPE_REAL);
	led_init();

	while (1)
	{
		printf("LEDS ON!!\n");
		led_set(LED_0, LED_STATE_ON);
		timer_delay(1000);
		led_set(LED_1, LED_STATE_ON);
		timer_delay(1000);
		led_set(LED_2, LED_STATE_ON);
		timer_delay(1000);
		led_set(LED_3, LED_STATE_ON);
		timer_delay(1000);

		printf("LEDS OFF!!\n");
		led_set(LED_0, LED_STATE_OFF);
		timer_delay(1000);
		led_set(LED_1, LED_STATE_OFF);
		timer_delay(1000);
		led_set(LED_2, LED_STATE_OFF);
		timer_delay(1000);
		led_set(LED_3, LED_STATE_OFF);
		timer_delay(1000);

	}
	return 0;
}



