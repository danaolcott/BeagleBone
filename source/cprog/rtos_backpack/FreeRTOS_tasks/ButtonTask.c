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

#include "DisplayTask.h"	//message struct and queue

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
//Trigger on falling
//
void ButtonTask(void *pvParameters)
{
	DisplayMessage msg;
	int n;
    printf("Running ButtonTask\n");
    uint8_t buttonValue = 0x00;
    uint8_t buttonFlag = 0x00;

    for(;;)
    {

    	/*
        //read left
        buttonValue = button_read(BUTTON_TYPE_LEFT);
        if (!buttonValue)
        {
        	printf("Button Left Pressed\n");
        	buttonFlag = 1;

        	//post a message to the display to show something
        	memset(msg.buffer, 0x00, DISPLAY_BUFFER_SIZE);
        	n = sprintf(msg.buffer, "Left Button");
        	msg.sig = DISPLAY_SIG_WRITE_LINE_BYTES;
        	msg.length = n;
        	msg.line = 2;

        	//post message to queue with no waiting
        	xQueueSend(DisplayQueue, &msg, portMAX_DELAY);
        }

*/



        buttonValue = button_read(BUTTON_TYPE_CENTER);
        if (!buttonValue)
        {
        	printf("Center Button Pressed\n");
        	buttonFlag = 1;

        	//post a message to the display to show something
        	memset(msg.buffer, 0x00, DISPLAY_BUFFER_SIZE);
        	n = sprintf(msg.buffer, "Center Button");
        	msg.sig = DISPLAY_SIG_WRITE_LINE_BYTES;
        	msg.length = n;
        	msg.line = 2;

        	//post message to queue with no waiting
        	xQueueSend(DisplayQueue, &msg, portMAX_DELAY);
        }

        buttonValue = button_read(BUTTON_TYPE_RIGHT);
        if (!buttonValue)
        {
        	printf("Right Button Pressed\n");
        	buttonFlag = 1;

        	//post a message to the display to show something
        	memset(msg.buffer, 0x00, DISPLAY_BUFFER_SIZE);
        	n = sprintf(msg.buffer, "Right Button");
        	msg.sig = DISPLAY_SIG_WRITE_LINE_BYTES;
        	msg.length = n;
        	msg.line = 2;

        	//post message to queue with no waiting
        	xQueueSend(DisplayQueue, &msg, portMAX_DELAY);
        }

        //test the flag for if a button was pressed, if
        //so then clear the message
        if (buttonFlag)
        {
        	vTaskDelay(200);

			memset(msg.buffer, 0x00, DISPLAY_BUFFER_SIZE);
			n = sprintf(msg.buffer, "________________");
			msg.sig = DISPLAY_SIG_WRITE_LINE_BYTES;
			msg.length = n;
			msg.line = 2;

			//post message to queue with no waiting
			xQueueSend(DisplayQueue, &msg, portMAX_DELAY);

        	buttonFlag = 0;
        }

    	vTaskDelay(10);
    }

    //clean exit
    vTaskDelete(NULL);
}


