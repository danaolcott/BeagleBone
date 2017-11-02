//////////////////////////////////////////////////
/* 9/29/17 and 10/31/17
 * Dana Olcott
 * gpio_driver.h
 * Header file for gpio driver using the /dev/
 * file system, for use with pins 26, 27... 60?
 * and what ever other pins are needed
 *
 * Assumes device driver files in /devices/pin26 and
 * /devices/pin27 are loaded first

 */
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#define GPIO_26_PATH		((char*)"/dev/pin26");
#define GPIO_27_PATH		((char*)"/dev/pin27");

typedef enum
{
	GPIO_PIN_26,
	GPIO_PIN_27,
}GPIOPin_t;

typedef enum
{
	GPIO_STATE_OFF = 0,
	GPIO_STATE_ON = 1,
	GPIO_STATE_TOGGLE = 2,

}GPIOState_t;


void gpio_init(void);
void gpio_set(GPIOPin_t pin, GPIOState_t state);
GPIOState_t gpio_get_state(GPIOPin_t pin);

#endif
