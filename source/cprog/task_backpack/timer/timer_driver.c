////////////////////////////////////////////////
/* 9/24/17
 * Dana Olcott
 * timer_driver
 *
 * Interval Timer running in Linux time.  There are
 * three types of timers we can use, real, virtual, and
 * prof?  real - includes time for the timeout process as
 * as well as other processes when computing the timeout.
 * virtual - includes only the time for the timer process
 * when computing timeout value (ie, timeouts should take
 * longer for virtual timer).  Not sure what prof timer is
 * for.
 *
 * Init timer from main, timer_init().  This starts the timer
 * and tick++.
 * timer_delay(ms) - delay in ms.  Note:  the timebase
 * appears to be off by factor of 10.  1000ms is actually 10
 * seconds.  Not sure why.
 *
 * The setup started from the following web source:
 * //http://www.informit.com/articles/article.aspx?p=23618&seqNum=14
 *
 */

#include <signal.h>		//timer
#include <string.h>		//memset
#include <sys/time.h>	//interval timer

#include "timer_driver.h"
#include "task.h"		//task controller isr

///////////////////////////////////////
//timer tick
volatile unsigned long tick;
int mTimerType = TIMER_TYPE_REAL;


/////////////////////////////////////
//timer_handler()
//Timer Callback function
//Note: timer_delay function resets
//timer tick value to avoid rolling over
//
//When using the task controller, task.c/.h
//call the Task_TimerHandlerISR here
void timer_handler(void)
{
	tick++;

	Task_TimerISRHandler();
}

////////////////////////////////////////////
//Timer init
//See example from:
//http://www.informit.com/articles/article.aspx?p=23618&seqNum=14
//
//Seems like I can only get this to 100ms accurracy
//the timebase is off, I have to divide the delay / 10
//to make it so it's close.
//
//Note: don't forget to call timer_init because it
//it controls the tick value.  If timer_delay is
//called, it will never return.
//
//Pass it TIMER_TYPE_REAL or TIMER_TYPE_VIRTUAL
//to set how the timer callback is determined
//or computed.
//
//
void timer_init(int timerType)
{
	tick = 0;				//reset the tick

	if ((timerType == TIMER_TYPE_REAL) || (timerType == TIMER_TYPE_VIRTUAL))
		mTimerType = timerType;
	else
		mTimerType = TIMER_TYPE_DEFAULT;


	struct sigaction sa;
	struct itimerval timer;

	memset(&sa, 0x00, sizeof(sa));
	sa.sa_handler = &timer_handler;		//map timer handler function

	//configure the signal based on the timer
	//type:  Real timer gets a "SIGALRM",
	//a virtural timer gets a "SIGVTALRM
	if (mTimerType == TIMER_TYPE_VIRTUAL)
		sigaction(SIGVTALRM, &sa, NULL);	//virtual
	else if (mTimerType == TIMER_TYPE_REAL)
		sigaction(SIGALRM, &sa, NULL);	//real
	//default virtual
	else
		sigaction(SIGVTALRM, &sa, NULL);	//virtual

	//Configure timer to timeout at 1ms

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 1000;		//1ms tick rate

	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 1000;

	//start the timer
	if (mTimerType == TIMER_TYPE_VIRTUAL)
		setitimer(ITIMER_VIRTUAL, &timer, NULL);	//virtual
	else if (mTimerType == TIMER_TYPE_REAL)
		setitimer(ITIMER_REAL, &timer, NULL);		//read
	else
		setitimer(ITIMER_VIRTUAL, &timer, NULL);	//virtual


}


unsigned long timer_getTick(void)
{
	return tick;
}

//////////////////////////////////////////////
//timer_delay(ms)
//
//Delay period needs to be adjusted based on
//the timer time.  For virtual timers, use
//a delay that is about 1/10 of the value passed
//into the function.  This probably depends on what
//other processes are running.  The 1/10 is based on
//observation with the scope.  When using a real timer
//the delay can be as is, no changed needed
//
void timer_delay(volatile unsigned long delay)
{
	volatile unsigned long value;

	tick = 0;	//reset the tick

	if (mTimerType == TIMER_TYPE_VIRTUAL)
		value = tick + (delay / 10);
	else if (mTimerType == TIMER_TYPE_REAL)
		value = tick + delay;
	else
		value = tick + (delay / 10);		//error

	while (tick < value){};

}
