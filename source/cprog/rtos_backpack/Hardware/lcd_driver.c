////////////////////////////////////////////
/*
11/1/17
Dana Olcott

LCD Driver File for the WDOGXXXX 16x3 line
character display from EA Displays.  The LCD
is wired for a 4bit interface as shown in the 
schmatics for the BeagleBone backpack located in
github source/backpack

The following modules must be loaded prior to 
using this file:
lcd_interface
sysfs_lcd

LCD control is via sys/kernel/lcd, and includes
the following files for read/write:

line0, line1, line2
contrast
cursor

Lines are 16 characters long, if you try to write more
or less, it truncates or fills remaining with spaces




*/
//////////////////////////////////////////////

#include "lcd_driver.h"
#include <stdlib.h>		//system call to run spi_config
#include <fcntl.h>		//O_RDWR
#include <unistd.h>		//open, close, read, write... etc
#include <string.h>



void lcd_init(void)
{
	lcd_setContrast(11);
	lcd_cursorDisable();

	lcd_writeLine(0, "RTOS Line 0");
	lcd_writeLine(1, "RTOS Line 1");
	lcd_writeLine(2, "RTOS Line 2");

}

void lcd_setContrast(uint8_t contrast)
{
	int fp, n;
	char buff[10];
	memset(buff, 0x00, 10);

	//printf contrast into contrast file
	n = sprintf(buff, "%d", contrast);

	fp = open(LCD_PATH_CONTRAST, O_RDWR);	//open for read/write
	write(fp, buff, n);
	close(fp);
}


//////////////////////////////////
//read contents of the contrast file
uint8_t lcd_getContrast(void)
{
	int fp, bytesRead;
	int value = 0x00;
	char buff[10];
	memset(buff, 0x00, 10);

	fp = open(LCD_PATH_CONTRAST, O_RDWR);	//open for read/write
	bytesRead = read(fp, buff, 10);		//read value, max buffer 10
	close(fp);							//close

	if (bytesRead > 0)
		value = atoi(buff);

	return ((uint8_t)value);
}

void lcd_cursorEnable(void)
{
	int fp, n;
	char buff[10];
	memset(buff, 0x00, 10);

	//printf 1 into cursor file
	n = sprintf(buff, "1");

	fp = open(LCD_PATH_CURSOR, O_RDWR);	//open for read/write
	write(fp, buff, n);
	close(fp);
}

void lcd_cursorDisable(void)
{
	int fp, n;
	char buff[10];
	memset(buff, 0x00, 10);

	//printf 0 into cursor file
	n = sprintf(buff, "0");

	fp = open(LCD_PATH_CURSOR, O_RDWR);	//open for read/write
	write(fp, buff, n);
	close(fp);
}


void lcd_writeLine(uint8_t line, char* buffer)
{
	int fp;
	int length = strlen(buffer);

	if (length > 16)
		length = 16;

	char* path = LCD_PATH_LINE0;

	if (!line)
		path = LCD_PATH_LINE0;
	else if (line == 1)
		path = LCD_PATH_LINE1;
	else if (line == 2)
		path = LCD_PATH_LINE2;

	if ((line >= 0) && (line < 3))
	{
		fp = open(path, O_RDWR);	//open for read/write
		write(fp, buffer, length);
		close(fp);
	}
}


//////////////////////////////////////////////////
//lcd_writeLineBytes
//Same as writeLine, but with additional
//argument for the length in bytes.
//if bytes > 16, just writes the first 16
//bytes.
void lcd_writeLineBytes(uint8_t line, char* buffer, uint8_t length)
{
	int fp;

	if (length > 16)
		length = 16;

	char* path = LCD_PATH_LINE0;

	if (!line)
		path = LCD_PATH_LINE0;
	else if (line == 1)
		path = LCD_PATH_LINE1;
	else if (line == 2)
		path = LCD_PATH_LINE2;

	if ((line >= 0) && (line < 3))
	{
		fp = open(path, O_RDWR);	//open for read/write
		write(fp, buffer, length);
		close(fp);
	}
}
