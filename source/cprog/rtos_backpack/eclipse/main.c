/*
 * main.c
 *
 *  Created on: Oct 31, 2017
 *      Author: danao
 */



#include "main.h"


/* Just used to count the number of times the example task callback function is
called, and the number of times a queue send passes. */
//static unsigned long uxCheckTaskHookCallCount = 0;
static unsigned long uxQueueSendPassedCount = 0;
//static int iSerialReceive = 0;


int main( void )
{
	//init the hardware
	gpio_init();		//ok, pins 26 and 27
	led_init();			//ok, pins 26 and 27
	eeprom_init();		//do nothing for now
	lcd_init();
	button_init();


	//create the other tasks
	LedTask_Init();			//led task
	DisplayTask_Init();		//4bit display
	ButtonTask_Init();		//button polling


	/* Create the co-routines that communicate with the tick hook. */
	vStartHookCoRoutines();

	/* Set the scheduler running.  This function will not return unless a task calls vTaskEndScheduler(). */
	vTaskStartScheduler();

	return 1;
}

void vMainQueueSendPassed( void )
{
	/* This is just an example implementation of the "queue send" trace hook. */
	uxQueueSendPassedCount++;
}
/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/


void vApplicationIdleHook( void )
{
	/* The co-routines are executed in the idle task using the idle task hook. */
	vCoRoutineSchedule();	/* Comment this out if not using Co-routines. */

#ifdef __GCC_POSIX__
	struct timespec xTimeToSleep, xTimeSlept;
		/* Makes the process more agreeable when using the Posix simulator. */
		xTimeToSleep.tv_sec = 1;
		xTimeToSleep.tv_nsec = 0;
		nanosleep( &xTimeToSleep, &xTimeSlept );
#endif
}
/*-----------------------------------------------------------*/

