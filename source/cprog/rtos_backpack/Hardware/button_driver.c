////////////////////////////////////////////////
/*
11/1/17
Dana Olcott
Button driver files for user buttons on the
beaglebone backpack board.   Uses glib to
capture button changes

Board has 3 user buttons on the following GPIOs:
GPIO112 - button left
GPIO115 - button center
GPIO61 - button right

*/

#include <stdio.h>
#include <fcntl.h>		//open, close, read, write O_RDWR
#include <string.h>
#include "button_driver.h"



void button_init(void)
{


}

uint8_t button_read(ButtonType_t button)
{
	int fp, bytesRead;
	int value = 0x00;
	char buff[10];
	memset(buff, 0x00, 10);

	char* path  = BUTTON_PATH_LEFT;

	switch(button)
	{
		case BUTTON_TYPE_LEFT:
			path = BUTTON_PATH_LEFT;
			break;
		case BUTTON_TYPE_CENTER:
			path = BUTTON_PATH_CENTER;
			break;
		case BUTTON_TYPE_RIGHT:
			path = BUTTON_PATH_RIGHT;
			break;
		default:
			path = BUTTON_PATH_LEFT;
			break;
	}

	fp = open(path, O_RDWR);			//open for read/write
	bytesRead = read(fp, buff, 10);		//read value, max buffer 10
	close(fp);							//close

	if (bytesRead > 0)
		value = atoi(buff);

	return ((uint8_t)value);
}


