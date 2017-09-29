//////////////////////////////////////////////////
/* 9/24/17
 * Dana Olcott
 * timer_driver.h
 * Header file for timer that uses the Linux
 * Interval timer.
 */
#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

//defines timer type
#define TIMER_TYPE_VIRTUAL	((int)1)
#define TIMER_TYPE_REAL		((int)2)
#define TIMER_TYPE_DEFAULT	TIMER_TYPE_VIRTUAL

//prototypes
void timer_handler(void);

void timer_init(int timerType);
unsigned long timer_getTick(void);
void timer_delay(volatile unsigned long delay);

#endif
