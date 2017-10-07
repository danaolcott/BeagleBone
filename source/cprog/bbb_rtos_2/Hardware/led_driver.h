//////////////////////////////////////////////////
/* 9/24/17
 * Dana Olcott
 * led_driver.h
 * Header file for led driver using the /dev/
 * file system.  
 */
#ifndef LED_DRIVER_H
#define LED_DRIVER_H


//device file paths for each user led
//should be able to read, write, cat, echo with these paths.
#define LED_0_PATH		((char*)"/sys/devices/platform/leds/leds/beaglebone:green:usr0/brightness")
#define LED_1_PATH		((char*)"/sys/devices/platform/leds/leds/beaglebone:green:usr1/brightness")
#define LED_2_PATH		((char*)"/sys/devices/platform/leds/leds/beaglebone:green:usr2/brightness")
#define LED_3_PATH		((char*)"/sys/devices/platform/leds/leds/beaglebone:green:usr3/brightness")

typedef enum
{
	LED_0,
	LED_1,
	LED_2,
	LED_3,

}LedType_t;

typedef enum
{
	LED_STATE_OFF = 0,
	LED_STATE_ON = 1,
}LedState_t;


void led_init(void);

void led_set(LedType_t led, LedState_t state);
LedState_t led_get_state(LedType_t led);
LedState_t led_toggle(LedType_t led);

//void led_on(LedType_t led);
//void led_off(LedType_t led);
//void led_toggle(LedType_t led);

#endif
