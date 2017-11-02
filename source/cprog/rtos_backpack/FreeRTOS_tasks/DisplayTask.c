/*
DisplayTask Definition

DisplayTask controls what is being displayed
on the lcd.  FOr now, have it simply
read the contents of eeprom and display it

*/

#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////
#include "DisplayTask.h"	//task level

#include "lcd_driver.h"		//for testing the lcd interface

/////////////////////////////////////////////////
//task and handle defs
static void DisplayTask(void *pvParameters);

//task handles
xTaskHandle DisplayTaskHandle = NULL;

//queue definition
xQueueHandle DisplayQueue;


/////////////////////////////////////////////////
//DisplayTask_Init
void DisplayTask_Init()
{
	//task
	xTaskCreate(DisplayTask, (const portCHAR *)"disp_task", configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 1, &DisplayTaskHandle);

	//queue - queue of DisplayMessage structs
	DisplayQueue= xQueueCreate(10, sizeof(DisplayMessage));

	printf("DisplayTask_Init()\n");
}


//////////////////////////////////////////////////////
//DisplayTask
//Write/Read the contents of eeprom and write to
//the display every 2 seconds
//
void DisplayTask(void *pvParameters)
{
	DisplayMessage msg;

    for(;;)
    {
        if (pdPASS == xQueueReceive(DisplayQueue, &msg, portMAX_DELAY))
        {
        	switch(msg.sig)
        	{
        		case DISPLAY_SIG_CLEAR:
        		{
        			lcd_writeLine(0, "________________");
        			lcd_writeLine(1, "________________");
        			lcd_writeLine(2, "________________");
        			break;
        		}
        		case DISPLAY_SIG_WRITE_LINE:
        		{
        			lcd_writeLine(msg.line, (char*)msg.buffer);
        			break;
        		}
        		case DISPLAY_SIG_WRITE_LINE_BYTES:
        		{
        			lcd_writeLineBytes(msg.line, (char*)msg.buffer, msg.length);
        			break;
        		}
        		default:
        		{
        			printf("Invalid Signal\n");
        			break;
        		}
        	}
        }
    }

    //clean exit
    vTaskDelete(NULL);
}


