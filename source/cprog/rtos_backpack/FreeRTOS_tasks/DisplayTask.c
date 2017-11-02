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
#include "eeprom_driver.h"	//for testing the eeprom interface
#include "lcd_driver.h"		//for testing the lcd interface

/////////////////////////////////////////////////
//task and handle defs
static void DisplayTask(void *pvParameters);

//task handles
xTaskHandle DisplayTaskHandle = NULL;

/////////////////////////////////////////////////
//DisplayTask_Init
void DisplayTask_Init()
{
	xTaskCreate(DisplayTask, (const portCHAR *)"disp_task", configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 1, &DisplayTaskHandle);

	printf("DisplayTask_Init()\n");
}


//////////////////////////////////////////////////////
//DisplayTask
//Write/Read the contents of eeprom and write to
//the display every 2 seconds
//
void DisplayTask(void *pvParameters)
{
	int n = 0;
	uint8_t dataWrite = 0x00;
	uint8_t dataRead = 0x00;
	uint16_t address = 0x00;
	char buffer[32];

	printf("Running DisplayTask\n");

    for(;;)
    {
        //write dataWrite into address
        eeprom_writeData(address, dataWrite);
        printf("EEPROM Write -> ADD: 0x%04x, DATA: 0x%02x\n",
        address, dataWrite);

        //read the data back
        dataRead = eeprom_readData(address);
        printf("EEPROM Read  -> ADD: 0x%04x, DATA: 0x%02x\n\n",
        address, dataRead);

        //display the contents on the lcd
        lcd_writeLine(0, "ADDR/WRITE/READ");

        memset(buffer, 0x00, 32);
        n = sprintf(buffer, "0x%04x,0x%02x,0x%02x", address, dataWrite, dataRead);
        lcd_writeLineBytes(1, buffer, n);

        dataWrite++;
        address++;

        vTaskDelay(2000);
    
    }

    //clean exit
    vTaskDelete(NULL);
}


