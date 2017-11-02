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
extern xTaskHandle LedTaskHandle;


//function prototypes
void LedTask_Init(void);


#endif
