/*
LED Task

*/

#ifndef LED_TASK__H
#define LED_TASK__H

#include "priorities.h"
#include "FreeRTOS.h"		//rtos
#include "task.h"			//rtos
#include "queue.h"			//rtos
#include "FreeRTOSConfig.h"


//externs - access to the outside world
extern xTaskHandle LedControllerTaskHandle;
extern xTaskHandle LedTaskHandle;


//function prototypes
void LedTask_Init(void);

void LedTask_Create(void);
void LedTask_Destroy(void);


#endif
