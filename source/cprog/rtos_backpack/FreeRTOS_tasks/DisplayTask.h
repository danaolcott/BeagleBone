/*
Display Task

*/

#ifndef DISPLAY_TASK__H
#define DISPLAY_TASK__H

#include <stdint.h>

#include "priorities.h"
#include "FreeRTOS.h"		//rtos
#include "task.h"			//rtos
#include "queue.h"			//rtos
#include "FreeRTOSConfig.h"

#define DISPLAY_BUFFER_SIZE		32

//typedef enum display signal
typedef enum
{
	DISPLAY_SIG_CLEAR,
	DISPLAY_SIG_WRITE_LINE,
	DISPLAY_SIG_WRITE_LINE_BYTES,
}DisplaySignal_t;


//DisplayMessge
typedef struct
{
	DisplaySignal_t sig;					//signal
	uint8_t line;							//line to draw
	char buffer[DISPLAY_BUFFER_SIZE];	//display buffer
	uint8_t length;							//length of text

}DisplayMessage;


//externs - access to the outside world
extern xTaskHandle DisplayTaskHandle;

//queue definition
extern xQueueHandle DisplayQueue;


//function prototypes
void DisplayTask_Init(void);


#endif
