/*
Button Left Task

*/

#ifndef BUTTON_LEFT_TASK__H
#define BUTTON_LEFT_TASK__H

#include "priorities.h"
#include "FreeRTOS.h"		//rtos
#include "task.h"			//rtos
#include "queue.h"			//rtos
#include "FreeRTOSConfig.h"


//externs - access to the outside world
extern xTaskHandle ButtonLeftTaskHandle;


//function prototypes
void ButtonLeftTask_Init(void);


#endif
