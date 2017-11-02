/*
Button Task

*/

#ifndef BUTTON_TASK__H
#define BUTTON_TASK__H

#include "priorities.h"
#include "FreeRTOS.h"		//rtos
#include "task.h"			//rtos
#include "queue.h"			//rtos
#include "FreeRTOSConfig.h"


//externs - access to the outside world
extern xTaskHandle ButtonTaskHandle;


//function prototypes
void ButtonTask_Init(void);


#endif
