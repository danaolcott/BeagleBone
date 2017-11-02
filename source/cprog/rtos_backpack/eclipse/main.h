/*
 * main.h
 *
 *  Created on: Oct 31, 2017
 *      Author: danao
 */

#ifndef MAIN_H_
#define MAIN_H_

/* System headers. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>


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
#include "gpio_driver.h"	//pin26 and pin27 - read/write to /dev/pin26 / 27
#include "led_driver.h"		//pin26 and pin27 - read/write to /dev/pin26 / 27
#include "eeprom_driver.h"	//eeprom ic, read/write to sys/kernel/eeprom
#include "lcd_driver.h"		//lcd, read/write to sys/kernel/lcd

//tasks
#include "LedTask.h"
#include "DisplayTask.h"



#endif /* MAIN_H_ */
