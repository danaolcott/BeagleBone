////////////////////////////////////////////////
/* 10/10/17
 * Dana Olcott
 * button_driver

User space control functions for accessing the
sysfs_button module.  The button is assumed  to 
control a toggle switch that toggles an led 
connected to P9-12.  It is assumed that the module
is installed and works properly.  The files are created
using the kobject and has two attributes:

presses - number of times the button was pressed
state  - state of the switch - 0 or 1.  

Each can be read/written to.  Toggle the switch using
the button press, or by writing 0, 1, or 2 into the
state attribute.  0 = off, 1 = on, 2 = toggle


*/
///////////////////////////////////////////////

#include "button_driver.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		//O_RDWR
#include <unistd.h>		//open, close, read, write... etc
#include <string.h>



void button_init(void)
{
	button_setNumPresses(0);		//reset the button press count
	button_setState(0);				//off the switch
}


//button presses - read
uint16_t button_getNumPresses(void)
{
	char* path = BUTTON_PRESSES_PATH;
	int fp, value, bytesRead;

	char buff[16];
	memset(buff, 0x00, 16);

	fp = open(path, O_RDWR);			//open
	bytesRead = read(fp, buff, 16);		//read into buff, max 10 bytes
	close(fp);							//close

	if (bytesRead > 0)
	{
		value = atoi(buff);
	}

	else
		value = 0;

	return value;	
}

void button_setNumPresses(uint16_t num)
{
	char* path = BUTTON_PRESSES_PATH;
	int fp, bytesWritten, bytesToWrite;

	char buff[10];
	memset(buff, 0x00, 10);

	if (num < 0)
		num = 0;

	bytesToWrite = sprintf(buff, "%d\n", num);

	fp = open(path, O_RDWR);			//open
	bytesWritten = write(fp, buff, bytesToWrite);
	close(fp);

}


uint16_t button_getState(void)
{
	char* path = BUTTON_STATE_PATH;
	int fp, value, bytesRead;

	char buff[16];
	memset(buff, 0x00, 16);

	fp = open(path, O_RDWR);			//open
	bytesRead = read(fp, buff, 16);		//read into buff, max 10 bytes
	close(fp);							//close

	if (bytesRead > 0)
	{
		value = atoi(buff);
	}
	
	else
		value = 0;

	return value;	
}

void button_setState(uint16_t state)
{
	char* path = BUTTON_STATE_PATH;
	int fp, bytesWritten, bytesToWrite;

	char buff[10];
	memset(buff, 0x00, 10);

	//state - 0 = off, 1 = on, 2 = toggle
	if (state < 0)
		state = 0;
	else if (state > 2)
		state = 0;

	bytesToWrite = sprintf(buff, "%d\n", state);

	fp = open(path, O_RDWR);			//open
	bytesWritten = write(fp, buff, bytesToWrite);
	close(fp);

}



