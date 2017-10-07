/*
AdcTask Definition

AdcTask contains two tasks:

AdcPollingTask:
Reads adc channels 0 and 1 every 2 seconds using vTaskDelay
Puts read values into a message struct and passes the 
message to the receiver task

AdcReceiverTask:
Runs each time it receives a message from the AdcPollingTask.
Toggle LED 3 every time it receives a message.

*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////
#include "AdcTask.h"		//task level
#include "adc_driver.h"		//hardware level
#include "led_driver.h"		//toggle led.

/////////////////////////////////////////
//task and handle defs
static void AdcPollingTask(void *pvParameters);
static void AdcReceiveTask(void *pvParameters);

/////////////////////////////////////////
//task handles
xTaskHandle AdcPollingTaskHandle = NULL;
xTaskHandle AdcReceiveTaskHandle = NULL;

//////////////////////////////////////////
//Queue Handles
xQueueHandle AdcQueue;

/////////////////////////////////////////////
//member variables
uint16_t adcRawValue_Ch0 = 0x00;
uint16_t adcRawValue_Ch1 = 0x00;

uint16_t adcMilliVolts_Ch0 = 0x00;
uint16_t adcMilliVolts_Ch1 = 0x00;


////////////////////////////////////////////////////
//AdcTask_Init
//
void AdcTask_Init()
{
	//init member variables
	adcRawValue_Ch0 = 0x00;
	adcRawValue_Ch1 = 0x00;
	adcMilliVolts_Ch0 = 0x00;
	adcMilliVolts_Ch1 = 0x00;

    printf("AdcTask_Init()\n");

	//AdcPolling task
	xTaskCreate(AdcPollingTask, (const portCHAR *)"adc_poll", configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 1, &AdcPollingTaskHandle);

    //AdcReceive Task
    xTaskCreate(AdcReceiveTask, (const portCHAR *)"adc_rx", configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 1, &AdcReceiveTaskHandle);

	//Queue - AdcMessage Structs
	AdcQueue = xQueueCreate(10, sizeof(AdcMessage));

}


//////////////////////////////////////////////////////
//AdcPollingTask
//
void AdcPollingTask(void *pvParameters)
{
	AdcMessage msg;				//signal, adc ch0, ch1

    for(;;)
    {
    	printf("AdcPollingTask\n");

    	msg.sig = ADC_SIG_UPDATE;
    	msg.adcRawValueCh0 = adc_readChannel_raw(ADC_CH0);
    	msg.adcRawValueCh1 = adc_readChannel_raw(ADC_CH1);

    	//post message to queue with no waiting
    	xQueueSend(AdcQueue, &msg, portMAX_DELAY);

    	vTaskDelay(2000);
    }

    //clean exit
    vTaskDelete(NULL);
}


//////////////////////////////////////////////////////
//AdcReceiveTask
//Task runs when it receives a message from
//the polling task.
void AdcReceiveTask(void *pvParameters)
{
	AdcMessage msg;

    for(;;)
    {
        if (pdPASS == xQueueReceive(AdcQueue, &msg, portMAX_DELAY))
        {
        	led_toggle(LED_3);

        	switch(msg.sig)
        	{
        		case ADC_SIG_UPDATE:
        			printf("ADC_SIG_UPDATE\n");
        			printf("ADC CH0: %d, CH1: %d\n", msg.adcRawValueCh0,
        					msg.adcRawValueCh1);

        			//update the adc readings
        			AdcTask_UpdateADCReadings(msg);

        			break;

        		default:
        			printf("Invalid Signal\n");
        			break;
        	}
        }
    }

    //clean exit
    vTaskDelete(NULL);
}


//////////////////////////////////////////
//ADC readings range from 0 to 4095 for 0 to 1.8v
//
void AdcTask_UpdateADCReadings(AdcMessage msg)
{
	adcRawValue_Ch0 = msg.adcRawValueCh0;
	adcRawValue_Ch1 = msg.adcRawValueCh1;

	adcMilliVolts_Ch0 = adcRawValue_Ch0 * 1800 / 4095;
	adcMilliVolts_Ch1 = adcRawValue_Ch1 * 1800 / 4095;
}

