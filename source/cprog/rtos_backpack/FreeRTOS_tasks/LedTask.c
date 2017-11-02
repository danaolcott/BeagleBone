/*
LedTask Definition

LedTask flashes led blue and red alternating
located on GPIO Pins 26 and 27.

*/

#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////
#include "LedTask.h"		//task level
#include "led_driver.h"		//hardware level

/////////////////////////////////////////////////
//task and handle defs
static void LedTask(void *pvParameters);

/////////////////////////////////////////////////
//member variables
LedState_t ledState = LED_STATE_OFF;

//task handles
xTaskHandle LedTaskHandle = NULL;

/////////////////////////////////////////////////
//LedTask_Init
//Build the queue and task
void LedTask_Init()
{
	ledState = LED_STATE_OFF;

	//start the controller task
	xTaskCreate(LedTask, (const portCHAR *)"led_task", configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 1, &LedTaskHandle);

	printf("LedTask_Init()\n");
}




//////////////////////////////////////////////////////
//LedTask
//Flash LEDs Blue and Red
//
void LedTask(void *pvParameters)
{
	printf("Running LedTask\n");

    for(;;)
    {
    	led_set(LED_BLUE, LED_STATE_ON);
    	led_set(LED_RED, LED_STATE_OFF);
    	vTaskDelay(500);
    	led_set(LED_BLUE, LED_STATE_OFF);
    	led_set(LED_RED, LED_STATE_ON);
    	vTaskDelay(500);
    }

    //clean exit
    vTaskDelete(NULL);
}


