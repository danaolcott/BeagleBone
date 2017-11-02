//////////////////////////////////////////////////
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
//////////////////////////////////////////////////

#ifndef LCD_DRIVER__H
#define LCD_DRIVER__H

#include <stdio.h>
#include <stdint.h>

#define LCD_PATH_CONTRAST	((char*)"/sys/kernel/lcd/contrast")
#define LCD_PATH_CURSOR		((char*)"/sys/kernel/lcd/cursor")

#define LCD_PATH_LINE0		((char*)"/sys/kernel/lcd/line0")
#define LCD_PATH_LINE1		((char*)"/sys/kernel/lcd/line1")
#define LCD_PATH_LINE2		((char*)"/sys/kernel/lcd/line2")

void lcd_init(void);
void lcd_setContrast(uint8_t contrast);
uint8_t lcd_getContrast(void);
void lcd_cursorEnable(void);
void lcd_cursorDisable(void);

void lcd_writeLine(uint8_t line, char* buffer);
void lcd_writeLineBytes(uint8_t line, char* buffer, uint8_t length);

#endif

