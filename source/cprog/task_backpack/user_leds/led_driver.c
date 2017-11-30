////////////////////////////////////////////////
/* 9/24/17
 * Dana Olcott
 * led_driver
 *
 * Controller for the onboard user leds led0, 1, 2, 3


 */



#include "led_driver.h"

#include <stdio.h>
#include <fcntl.h>		//macro defintions
#include <unistd.h>		//open, close, read, write... etc

#include <string.h>


char* led0_path = "/sys/devices/platform/leds/leds/beaglebone:green:usr0/brightness";
char* led1_path = "/sys/devices/platform/leds/leds/beaglebone:green:usr1/brightness";
char* led2_path = "/sys/devices/platform/leds/leds/beaglebone:green:usr2/brightness";
char* led3_path = "/sys/devices/platform/leds/leds/beaglebone:green:usr3/brightness";



////////////////////////////////////////
//led_init
//set all leds off
//
//also set the trigger to none...
void led_init()
{
	led_set(LED_0, LED_STATE_OFF);
	led_set(LED_1, LED_STATE_OFF);
	led_set(LED_2, LED_STATE_OFF);
	led_set(LED_3, LED_STATE_OFF);
}


void led_set(LedType_t led, LedState_t state)
{
	char* ledPath = led0_path;
	int fp;
	char buff[10];
	memset(buff, 0x00, 10);

	if (state == LED_STATE_OFF)
		buff[0] = '0';
	else if (state == LED_STATE_ON)
		buff[0] = '1';

	switch(led)
	{
		case LED_0:
			ledPath = LED_0_PATH;
			break;
		case LED_1:
			ledPath = LED_1_PATH;
			break;
		case LED_2:
			ledPath = LED_2_PATH;
			break;
		case LED_3:
			ledPath = LED_3_PATH;
			break;
	}

	fp = open(ledPath, O_RDWR);		//open
	write(fp, buff, 2);				//write state, 1 byte + 0x00
	close(fp);						//close

}

LedState_t led_get_state(LedType_t led)
{
	char* ledPath = led0_path;	//read path
	int fp;						//file
	char buff[10];				//read buffer
	memset(buff, 0x00, 10);

	//set the file path
	switch(led)
	{
		case LED_0:
			ledPath = LED_0_PATH;
			break;
		case LED_1:
			ledPath = LED_1_PATH;
			break;
		case LED_2:
			ledPath = LED_2_PATH;
			break;
		case LED_3:
			ledPath = LED_3_PATH;
			break;
	}

	//read the led status
	fp = open(ledPath, O_RDWR);		//open
	read(fp, buff, 10);				//read status into buffer
	close(fp);						//close

	if (buff[0] == '0')
		return LED_STATE_OFF;
	else
		return LED_STATE_ON;
}


void led_on(LedType_t led)
{
	led_set(led, LED_STATE_ON);
}

void led_off(LedType_t led)
{
	led_set(led, LED_STATE_OFF);
}

void led_toggle(LedType_t led)
{
	LedState_t state = led_get_state(led);

	if (state == LED_STATE_ON)
		state = LED_STATE_OFF;
	else
		state = LED_STATE_ON;

	led_set(led, state);

}


