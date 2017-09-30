////////////////////////////////////////////////
/* 9/29/17
 * Dana Olcott
 * GPIO Driver for GPIN Pin 9-12 on the 
 * BeagleBoneBlack.  This driver uses the
 * char device /dev/pin9_12.  Make sure you do
 * insmode /dev/pin9_12 before running.
 */

#include <stdio.h>
#include <fcntl.h>		//open, close, read, write O_RDWR
#include <string.h>

#include "gpio_driver.h"


////////////////////////////////////////
//gpio_init
//set pin 9_12 off
void gpio_init()
{
	gpio_set(GPIO_PIN_9_12, GPIO_STATE_OFF);
}


void gpio_set(GPIOPin_t pin, GPIOState_t state)
{
	int fp;
	char buff[10];
	memset(buff, 0x00, 10);
	char* gpioPath;

	if (state == GPIO_STATE_OFF)
		buff[0] = '0';
	else if (state == GPIO_STATE_ON)
		buff[0] = '1';

	switch(pin)
	{
		case GPIO_PIN_9_12:
			gpioPath = GPIO_PATH_9_12;
			break;
		default:
			gpioPath = GPIO_PATH_9_12;
			break;
	}

	fp = open(gpioPath, O_RDWR);	//open
	write(fp, buff, 2);				//write state, 1 byte + 0x00
	close(fp);						//close

}

///////////////////////////////////////
//read the state of the pin via the file system
GPIOState_t gpio_get_state(GPIOPin_t pin)
{
	GPIOState_t state = GPIO_STATE_OFF;

	int fp;
	char buff[10];
	memset(buff, 0x00, 10);
	char* gpioPath;

	//set the path
	switch(pin)
	{
		case GPIO_PIN_9_12:
			gpioPath = GPIO_PATH_9_12;
			break;
		default:
			gpioPath = GPIO_PATH_9_12;
			break;
	}

	fp = open(gpioPath, O_RDWR);	//open
	read(fp, buff, 1);		//read, size = # bytes read
	close(fp);						//close

	if (buff[0] == '0')
		state = GPIO_STATE_OFF;
	else if (buff[0] == '1')
		state = GPIO_STATE_ON;

	return state;
}

