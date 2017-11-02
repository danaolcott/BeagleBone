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
#include "eeprom_driver.h"
#include "lcd_driver.h"

#include "DisplayTask.h"	//posting messages

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
//Writes a value to eeprom and reads it back
//Sends a message to DisplayTask to display
//results on the lcd
void LedTask(void *pvParameters)
{
	DisplayMessage msg;

	int n = 0;
	uint8_t dataWrite = 0x00;
	uint8_t dataRead = 0x00;
	uint16_t address = 0x00;

	printf("Running LedTask\n");

    for(;;)
    {
    	led_set(LED_BLUE, LED_STATE_ON);
    	led_set(LED_RED, LED_STATE_OFF);
    	vTaskDelay(500);
    	led_set(LED_BLUE, LED_STATE_OFF);
    	led_set(LED_RED, LED_STATE_ON);
    	vTaskDelay(500);

        //write dataWrite into address
        eeprom_writeData(address, dataWrite);
        printf("EEPROM Write -> ADD: 0x%04x, DATA: 0x%02x\n",
        address, dataWrite);

        //read the data back
        dataRead = eeprom_readData(address);
        printf("EEPROM Read  -> ADD: 0x%04x, DATA: 0x%02x\n\n",
        address, dataRead);

    	//post a message to the display to show something
    	memset(msg.buffer, 0x00, DISPLAY_BUFFER_SIZE);
        n = sprintf(msg.buffer, "ADDR/WRITE/READ");
    	msg.sig = DISPLAY_SIG_WRITE_LINE_BYTES;
    	msg.length = n;
    	msg.line = 0;

    	//post message to queue with no waiting
    	xQueueSend(DisplayQueue, &msg, portMAX_DELAY);

    	//post a message to the display to show something
    	memset(msg.buffer, 0x00, DISPLAY_BUFFER_SIZE);
        n = sprintf(msg.buffer, "0x%04x,0x%02x,0x%02x", address, dataWrite, dataRead);
    	msg.sig = DISPLAY_SIG_WRITE_LINE_BYTES;
    	msg.length = n;
    	msg.line = 1;

    	//post message to queue with no waiting
    	xQueueSend(DisplayQueue, &msg, portMAX_DELAY);

        dataWrite++;
        address++;

        vTaskDelay(1000);
    }

    //clean exit
    vTaskDelete(NULL);
}


