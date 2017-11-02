/*
Display Task

*/

#ifndef DISPLAY_TASK__H
#define DISPLAY_TASK__H

#include "priorities.h"
#include "FreeRTOS.h"		//rtos
#include "task.h"			//rtos
#include "queue.h"			//rtos
#include "FreeRTOSConfig.h"


//externs - access to the outside world
extern xTaskHandle DisplayTaskHandle;


//function prototypes
void DisplayTask_Init(void);


#endif
