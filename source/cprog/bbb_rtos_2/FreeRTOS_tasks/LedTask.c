/*
LedTask Definition

LedTask contains two tasks:

LedTaskController:
Purpose is to create and destroy the led task

LedTask:
Purpose is to toggle led and print a message each
time the task starts.  The message is outside of the
loop so in theory, the only way to print the message
is to remove and create the task again.

*/

#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////
#include "LedTask.h"		//task level
#include "led_driver.h"		//hardware level

#include "fileIO.h"
#include "memory.h"

/////////////////////////////////////////
//task and handle defs
static void LedTaskController(void *pvParameters);
static void LedTask(void *pvParameters);

/////////////////////////////////////////////
//member variables
LedState_t ledState = LED_STATE_OFF;

#define WRITE_BUFFER_SIZE		2048
unsigned char writeBuffer[WRITE_BUFFER_SIZE];

//task handles
xTaskHandle LedTaskControllerHandle = NULL;
xTaskHandle LedTaskHandle = NULL;


////////////////////////////////////////////////////
//LedTask_Init
//Build the queue and task
void LedTask_Init()
{
	ledState = LED_STATE_OFF;
	memset(writeBuffer, 0x00, WRITE_BUFFER_SIZE);

	//start the controller task
	xTaskCreate(LedTaskController, (const portCHAR *)"led_cntrl", configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 1, &LedTaskControllerHandle);

	printf("LedTask_Init()\n");
}


//////////////////////////////////////////////////////
//LedTaskController
//Starts and stops the led task
//
void LedTaskController(void *pvParameters)
{
	unsigned char *ptr = NULL;

    for(;;)
    {
    	printf("Create LED Task\n");
		LedTask_Create();
    	vTaskDelay(3000);

    	//write the task list to the sd card, point
    	//ptr to the next available write buffer

    	ptr = memory_getActiveBuffer();
    	vTaskList((signed char*)ptr);

    	vAppendMessageToFile("/media/task_log.txt", "Contents of Task List - LED TASK ON\n");
    	vAppendMessageToFile("/media/task_log.txt", (const char*)ptr);

    	printf("Destroy LED Task\n");
    	LedTask_Destroy();
    	vTaskDelay(3000);

    	//write the task list to the sd card
    	//use writeBuffer
    	ptr = memory_getActiveBuffer();
    	vTaskList((signed char*)ptr);

    	vAppendMessageToFile("/media/task_log.txt", "Contents of Task List - LED TASK DESTROYED\n");
    	vAppendMessageToFile("/media/task_log.txt", (char*)ptr);

    }

    //clean exit
    vTaskDelete(NULL);
}



//////////////////////////////////////////////////////
//LedTask
//Simple flashes an led.  The task is suspended
//and resumed by the ledControllerTask
//
//Create and destroy this task over and over
//Strange, creating the task over and over again
//must not call delete task because the intial
//printf is not called.  However, if you call create, delete,
//create, etc., then the initial print statement is called.
//
//To get the task to run the intial steps,
//destory the task then create it again.
//
void LedTask(void *pvParameters)
{
	int counter = 0;
	printf("Running LedTask - Starting Loop: %d\n", counter);

    for(;;)
    {
    	led_set(LED_1, ledState);

    	if (ledState == LED_STATE_OFF)
    		ledState = LED_STATE_ON;
    	else
    		ledState = LED_STATE_OFF;

    	counter++;
    	vTaskDelay(500);
    }

    //clean exit
    vTaskDelete(NULL);
}



void LedTask_Create(void)
{
	//create the task if the led task handle is null
	if (LedTaskHandle == NULL)
	{
		xTaskCreate(LedTask, (const portCHAR *)"led", configMINIMAL_STACK_SIZE,
				NULL, tskIDLE_PRIORITY + 2, &LedTaskHandle);
	}

	else
	{
		//the handle is not null, delete and restart
		LedTask_Destroy();

		xTaskCreate(LedTask, (const portCHAR *)"led", configMINIMAL_STACK_SIZE,
				NULL, tskIDLE_PRIORITY + 2, &LedTaskHandle);
	}
}

void LedTask_Destroy(void)
{
	//check for non-existed led task handle
	if (LedTaskHandle != NULL)
	{
		vTaskDelete(LedTaskHandle);
		LedTaskHandle = NULL;
	}
}
