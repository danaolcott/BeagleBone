/*
 * main.h
 *
 *  Created on: Oct 4, 2017
 *      Author: danao
 */

#ifndef MAIN_H_
#define MAIN_H_

/* System headers. */
#include <stdio.h>
#include <stdint.h>

#include <time.h>				//already included. for time, date, etc
#include <sys/time.h>
#include <stdlib.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>			//open, close, read, write... etc

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

#include "FreeRTOSConfig.h"
#include "crhook.h"				//tick hook
#include "priorities.h"			//task priorities

//hardware level
#include "gpio_driver.h"
#include "led_driver.h"
#include "memory.h"			//read/write buffers
#include "adc_driver.h"		//adc channels
#include "spi_driver.h"		//spidev 1.0

//middle
#include "fileIO.h"

//tasks
#include "LedTask.h"
#include "AdcTask.h"









#endif /* MAIN_H_ */
