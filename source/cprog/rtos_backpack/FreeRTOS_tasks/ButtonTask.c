/*
ButtonTask Definition

BUttonTask polls for button state changes

I tried digging into how to get an interrupt
up to here, but having a hard time with it

*/

#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////
#include "ButtonTask.h"		//task level
#include "button_driver.h"	//hardware level

/////////////////////////////////////////////////
//task and handle defs
static void ButtonTask(void *pvParameters);

/////////////////////////////////////////////////
//member variables
uint16_t m_numPressesLeft = 0x00;
uint16_t m_numPressesCenter = 0x00;
uint16_t m_numPressesRight = 0x00;

//task handles
xTaskHandle ButtonTaskHandle = NULL;

/////////////////////////////////////////////////
//ButtonTask_Init
//Build the queue and task
void ButtonTask_Init()
{
	xTaskCreate(ButtonTask, (const portCHAR *)"btn_task", configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 1, &ButtonTaskHandle);

	printf("ButtonTask_Init()\n");
}




//////////////////////////////////////////////////////
//ButtonTask
//Poll Button States
//
void ButtonTask(void *pvParameters)
{
    printf("Running ButtonTask\n");
    uint8_t buttonValue = 0x00;

    for(;;)
    {

        //read left
        buttonValue = button_read(BUTTON_TYPE_LEFT);
        if (buttonValue)
        {
            //do something
        }

        buttonValue = button_read(BUTTON_TYPE_CENTER);
        if (buttonValue)
        {
            //do something
        }

        buttonValue = button_read(BUTTON_TYPE_RIGHT);
        if (buttonValue)
        {
            //do something
        }


    	vTaskDelay(200);
    }

    //clean exit
    vTaskDelete(NULL);
}


