//////////////////////////////////////////////////
/* 10/31/17
Dana Olcott
button_driver.h



 */
#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <stdint.h>

#define BUTTON_PATH_LEFT	((char*)"/sys/class/gpio/gpio112/value");
#define BUTTON_PATH_CENTER	((char*)"/sys/class/gpio/gpio115/value");
#define BUTTON_PATH_RIGHT	((char*)"/sys/class/gpio/gpio61/value");

typedef enum
{
	BUTTON_TYPE_LEFT,
	BUTTON_TYPE_CENTER,
	BUTTON_TYPE_RIGHT,

}ButtonType_t;


void button_init(void);

uint8_t button_read(ButtonType_t button);


#endif
