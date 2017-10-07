/*
ADC Task

*/

#ifndef ADC_TASK__H
#define ADC_TASK__H

#include "priorities.h"
#include "FreeRTOS.h"		//rtos
#include "task.h"			//rtos
#include "queue.h"			//rtos
#include "FreeRTOSConfig.h"


//AdcTask Signals
typedef enum
{
	ADC_SIG_UPDATE,			//update member variables

}AdcSignal_t;


typedef struct
{
	AdcSignal_t sig;
	uint16_t adcRawValueCh0;
	uint16_t adcRawValueCh1;

}AdcMessage;



//externs - access to the outside world
extern xTaskHandle AdcPollingTaskHandle;
extern xTaskHandle AdcReceiveTaskHandle;

//queue definition
extern xQueueHandle AdcQueue;

//function prototypes
void AdcTask_Init(void);

void AdcTask_UpdateADCReadings(AdcMessage msg);

#endif
