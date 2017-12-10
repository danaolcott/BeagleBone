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

#ifdef LINUX_SIM
char* led0_path = "./led0";
char* led1_path = "./led1";
char* led2_path = "./led2";
char* led3_path = "./led3";

#else

char* led0_path = "/sys/devices/platform/leds/leds/beaglebone:green:usr0/brightness";
char* led1_path = "/sys/devices/platform/leds/leds/beaglebone:green:usr1/brightness";
char* led2_path = "/sys/devices/platform/leds/leds/beaglebone:green:usr2/brightness";
char* led3_path = "/sys/devices/platform/leds/leds/beaglebone:green:usr3/brightness";

#endif



////////////////////////////////////////
//led_init
//inits the user leds on the BBB.  set all
//triggers to none and set initial states to
//off.  if debug on the linux sim, ignore the
//trigger, make a file for each led in the running
//directory.
void led_init()
{
#ifdef LINUX_SIM
	system("touch led0 led1 led2 led3");

	system("echo 0 > ./led0");
	system("echo 0 > ./led1");
	system("echo 0 > ./led2");
	system("echo 0 > ./led3");

#endif

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
			ledPath = led0_path;
			break;
		case LED_1:
			ledPath = led1_path;
			break;
		case LED_2:
			ledPath = led2_path;
			break;
		case LED_3:
			ledPath = led3_path;
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
			ledPath = led0_path;
			break;
		case LED_1:
			ledPath = led1_path;
			break;
		case LED_2:
			ledPath = led2_path;
			break;
		case LED_3:
			ledPath = led3_path;
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


