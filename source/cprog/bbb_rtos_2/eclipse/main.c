/*
 * main.c
 *
 *  Created on: Oct 4, 2017
 *      Author: danao
 */

#include "main.h"

/////////////////////////////////////////
//Tasks
static void myTask(void *pvParameters);

/* Just used to count the number of times the example task callback function is
called, and the number of times a queue send passes. */
//static unsigned long uxCheckTaskHookCallCount = 0;
static unsigned long uxQueueSendPassedCount = 0;
//static int iSerialReceive = 0;


int main( void )
{
	//init the hardware
	gpio_init();
	led_init();
	memory_init();
	adc_init();
	spi_init();				//pins 9-17, 9-18, 9-21, 9-22

	xTaskHandle myTaskHandle;
	xTaskCreate( myTask, "MyTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &myTaskHandle );

	//create the other tasks
	LedTask_Init();		//start 2 tasks
	AdcTask_Init();		//start 2 tasks


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



//////////////////////////////////////////////////
//myTask
void myTask( void *pvParameters )
{
	uint8_t data[] = {0xAA, 0xCC};

	printf("My Task - Entering loop\n");

	for ( ;; )
	{
		spi_writeArray(data, 2);
		vTaskDelay(200);
	}

	vTaskDelete( NULL );
}
