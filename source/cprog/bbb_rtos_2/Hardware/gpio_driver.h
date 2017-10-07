//////////////////////////////////////////////////
/* 9/29/17
 * Dana Olcott
 * gpio_driver.h
 * Header file for gpio driver using the /dev/
 * file system.  
 */
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H


#define GPIO_PATH_9_12		((char*)"/dev/pin9_12");


typedef enum
{
	GPIO_PIN_9_12,

}GPIOPin_t;

typedef enum
{
	GPIO_STATE_OFF = 0,
	GPIO_STATE_ON = 1,
}GPIOState_t;


void gpio_init(void);

void gpio_set(GPIOPin_t pin, GPIOState_t state);
GPIOState_t gpio_get_state(GPIOPin_t pin);

#endif
