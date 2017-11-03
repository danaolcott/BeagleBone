/*
ButtonLeftTask Definition

ButtonLeftTask uses poll.h to read state changes

This is a different approach to pushing interrupts
up to user space

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <signal.h>
#include <poll.h>


//////////////////////////////////////////////////
#include "ButtonLeftTask.h"	//task level
#include "button_driver.h"	//hardware level
#include "led_driver.h"
#include "DisplayTask.h"

/////////////////////////////////////////////////
//task and handle defs
static void ButtonLeftTask(void *pvParameters);


//task handles
xTaskHandle ButtonLeftTaskHandle = NULL;

/////////////////////////////////////////////////
//ButtonTask_Init
//Build the queue and task
void ButtonLeftTask_Init()
{
	xTaskCreate(ButtonLeftTask, (const portCHAR *)"btn_lt_task", configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 1, &ButtonLeftTaskHandle);

	printf("ButtonLeftTask_Init()\n");
}




//////////////////////////////////////////////////////
//ButtonLeftTask
//
//NOTE: poll only works when you can do edge in the sys/gpio
//folder.  It was showing up as none even though button
//interrupts were confingured.
//
void ButtonLeftTask(void *pvParameters)
{
	DisplayMessage msg;
	char* buttonPath  = BUTTON_PATH_LEFT;

	int f, n;
	struct pollfd poll_fds[1];
	char value[10];

	f = open(buttonPath, O_RDONLY);

	poll_fds[0].fd = f;
	poll_fds[0].events = POLLPRI | POLLERR;
	poll_fds[0].revents = POLLPRI | POLLERR;

	lseek(f, 0, SEEK_SET);    /* consume any prior interrupt */

    printf("Running ButtonLeftTask\n");

    for(;;)
    {
    	//trigger on falling - 0
    	//nfds = number of items to poll
    	//-1 = block indefinately until the event occurs
    	if (poll(poll_fds, 1, -1) > 0)
    	{
    		read(f, value, sizeof(value));
    		printf("Interrupt - Button Left Polling!!, Value: %c\n", value[0]);
    		lseek(f, 0, SEEK_SET);    /* consume any prior interrupt */

        	//post a message to the display to show something
        	memset(msg.buffer, 0x00, DISPLAY_BUFFER_SIZE);
        	n = sprintf(msg.buffer, "*Left Button*");
        	msg.sig = DISPLAY_SIG_WRITE_LINE_BYTES;
        	msg.length = n;
        	msg.line = 2;

        	//post message to queue with no waiting
        	xQueueSend(DisplayQueue, &msg, portMAX_DELAY);

        	for (n = 0 ; n < 8 ; n++)
        	{
        		led_set(LED_BLUE, LED_STATE_TOGGLE);
        		vTaskDelay(30);
        	}

        	//post a message to the display to show something
        	memset(msg.buffer, 0x00, DISPLAY_BUFFER_SIZE);
        	n = sprintf(msg.buffer, "________________");
        	msg.sig = DISPLAY_SIG_WRITE_LINE_BYTES;
        	msg.length = n;
        	msg.line = 2;

        	//post message to queue with no waiting
        	xQueueSend(DisplayQueue, &msg, portMAX_DELAY);

    	}

    	vTaskDelay(50);
    }

	close(f);

    //clean exit
    vTaskDelete(NULL);
}


