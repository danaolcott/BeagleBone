////////////////////////////////////////////////
/* 10/31/17
 * Dana Olcott
 * led_driver using pins 26 and 27
 *
 */



#include "led_driver.h"

#include <stdio.h>
#include <fcntl.h>		//O_RDWR
#include <unistd.h>		//open, close, read, write... etc
#include <string.h>


////////////////////////////////////////
//led_init
//set all leds off
void led_init()
{
	led_set(LED_BLUE, LED_STATE_OFF);
	led_set(LED_RED, LED_STATE_OFF);
}


void led_set(LedType_t led, LedState_t state)
{
	char* ledPath = LED_BLUE_PATH;
	int fp;
	char buff[10];
	memset(buff, 0x00, 10);

	if (state == LED_STATE_OFF)
		buff[0] = '0';
	else if (state == LED_STATE_ON)
		buff[0] = '1';
	else if (state == LED_STATE_TOGGLE)
		buff[0] = '2';

	switch(led)
	{
		case LED_BLUE:
			ledPath = LED_BLUE_PATH;
			break;
		case LED_RED:
			ledPath = LED_RED_PATH;
			break;
		default:
			ledPath = LED_BLUE_PATH;
			break;
	}

	fp = open(ledPath, O_RDWR);		//open
	write(fp, buff, 2);				//write state, 1 byte + 0x00
	close(fp);						//close

}

//////////////////////////////////////////////
//led_get_state
//Read the state of the led at the led dev path
//
LedState_t led_get_state(LedType_t led)
{
	LedState_t state = LED_STATE_OFF;
	char* ledPath = LED_BLUE;
	int fp, bytesRead;
	char buff[10];
	memset(buff, 0x00, 10);

	//set the path
	switch(led)
	{
		case LED_BLUE:
			ledPath = LED_BLUE_PATH;
			break;
		case LED_RED:
			ledPath = LED_RED_PATH;
			break;
		default:
			ledPath = LED_BLUE_PATH;
			break;
	}

	//read the state at the path
	fp = open(ledPath, O_RDWR);			//open
	bytesRead = read(fp, buff, 10);		//read value, max buffer 10
	close(fp);							//close

	if (bytesRead != 0)
	{
		if (buff[0] == '0')
			state = LED_STATE_OFF;
		else if (buff[0] == '1')
			state = LED_STATE_ON;
	}

	//TODO:
	//need to handle a case where it didn't read anything

	return state;
}


