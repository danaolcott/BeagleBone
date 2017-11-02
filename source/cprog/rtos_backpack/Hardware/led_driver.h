//////////////////////////////////////////////////
/* 9/24/17
 * Dana Olcott
 * led_driver.h
 * Header file for led driver using the /dev/
 * file system.  
 */
#ifndef LED_DRIVER_H
#define LED_DRIVER_H


//device file paths for user leds on pins 26 and 27
//paths provided by device drivers in the following:
///home/debian/devices/pin26
///home/debian/devices/pin27
//
//read, write, cat, echo with these paths.

#define LED_BLUE_PATH		((char*)"/dev/pin26")
#define LED_RED_PATH		((char*)"/dev/pin27")

typedef enum
{
	LED_BLUE,		//pin26
	LED_RED,		//pin27
}LedType_t;

typedef enum
{
	LED_STATE_OFF = 0,
	LED_STATE_ON = 1,
	LED_STATE_TOGGLE = 2,
}LedState_t;


void led_init(void);
void led_set(LedType_t led, LedState_t state);
LedState_t led_get_state(LedType_t led);

#endif
