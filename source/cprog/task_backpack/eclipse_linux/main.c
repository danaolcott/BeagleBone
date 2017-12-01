/*
 * main.c
 *
 *  Created on: Nov 29, 2017
 *      Author: danao
 *
 *
 *  The purpose of this program is to run a few
 *  tasks as a process on the BeagleBone.  This is a
 *  reuse of the task.c/.h files used for the msp430
 *  project.  The idea is to try to run two separate
 *  processes that use different time bases.
 *
 *  All tasks functions are run to completion and
 *  use the timebase defined in timer_driver.h, which
 *  uses a linux interval timer configured
 *  as TIMER_TYPE_REAL
 *
 *  Timebase is from timer_driver.  init the timer
 *  as TIMER_TYPE_REAL with a time tick of 1ms.
 *  Call the task isr handler function in the timer isr
 *
 *  Keep it simple by adding task function defs in
 *  main.  Control user onboard leds as the target
 *  (ie, the 4 small blue leds - these will need
 *  to get turned off in the startup script.)
 *
 */

#include <stdio.h>

#include "timer_driver.h"
#include "led_driver.h"

#include "task.h"

//defines
#define LINUX_SIM			1

#define TASK_TX_NAME		((char*)"tx")
#define TASK_RX_NAME		((char*)"rx")

//Task functions
void TaskFunction_Tx(void);
void TaskFunction_Rx(void);


int main()
{
	//init the timer
	timer_init(TIMER_TYPE_REAL);	//timebase for task.c/.h
	led_init();						//leds 0-3

	//add the tasks
	Task_AddTask(TASK_TX_NAME, TaskFunction_Tx, 500, 0);
	Task_AddTask(TASK_RX_NAME, TaskFunction_Rx, 100, 1);

	//start the scheduler
	Task_StartScheduler();

	while (1){};

	return 0;
}




//////////////////////////////////////////
//Task Definitions - Tx Task
//send values 0 - 3 to indicate
//toggle leds 0 to 3
void TaskFunction_Tx(void)
{
	TaskMessage msg = {TASK_SIG_TOGGLE, 0x00};
	static uint16_t i = 0;

	msg.value = i;

	uint8_t index = Task_GetIndexFromName("rx");

#ifdef LINUX_SIM
	printf("TX Task: index: %d, msg-sig: %d, msg-value: %d\n", index, msg.signal,
			msg.value);
#endif

	//send 0-3 with toggle signal
	Task_SendMessage(index, msg);

	if (i < 3)
		i++;
	else
		i = 0;

}


///////////////////////////////////////////
//Task Definitions - Rx Task
//The receiver task reads signal and value
//value is the led to toggle.  led0 - led3
//have enum values of 0 to 3
//
void TaskFunction_Rx(void)
{
	TaskMessage msg = {TASK_SIG_NONE, 0x00};
	uint8_t index = Task_GetIndexFromName("rx");

	//read into msg address
	while (Task_GetNextMessage(index, &msg) > 0)
	{
		//test the message value
		if ((msg.value >= LED_0) && (msg.value <= LED_3))
		{
			switch(msg.signal)
			{
				case TASK_SIG_ON:
				{
					led_on((LedType_t)msg.value);
					break;
				}
				case TASK_SIG_OFF:
				{
					led_off((LedType_t)msg.value);
					break;
				}

				case TASK_SIG_TOGGLE:
				{
					led_toggle((LedType_t)msg.value);
					break;
				}

				default:
					break;
			}
		}
	}
}



