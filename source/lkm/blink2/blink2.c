/*
//////////////////////////////////////////////////
//Linux Kernel Module Programming

Simple Flashing LED Example using Linux/gpio.h

The purpose of this module is to use /linux/gpio.h
to toggle the leds on the beaglebone backpack board
(see backpack for schematics/pcb express files).  
GPIO Pins are located on GPIO26 (8-14) and
GPIO27 (8-17).

The led will toggle using a workqueue
and posting delayed work.

GPIO26 - Blue Led on the backpack
GPIO27 - Red Led on the backpack

*/

#include <linux/init.h>       //entry/exit
#include <linux/module.h>     
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/workqueue.h>  //for timer

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("Toggle GPIO26 and GPIO27 Example");
MODULE_VERSION("0.1");



/////////////////////////////////////////////
//Globals for tracking state of the GPIO Pin 8-14
//Located on GPIO0, Pin26, GPIO Pin 26
//
static unsigned int ledBluePin = 26;      //pin 8-14
static unsigned int ledBlueState = 0;     //state 0 - off, 1 - on

static unsigned int ledRedPin = 27;      //pin 8-14
static unsigned int ledRedState = 0;     //state 0 - off, 1 - on


///////////////////////////////////////////
//Workqueue
//Method of setting up a delay to run a function.
//note: "my_work" does not need to be declated, it's
//a name.  you'll get an error if you declare it.
//
static void my_work_function(struct work_struct *unused);
static DECLARE_DELAYED_WORK(my_work, my_work_function);

/////////////////////////////////////////////////
//my_work_function
//Function called when delayed work timesout
//Toggle ledPin
//
static void my_work_function(struct work_struct *unused)
{
   if (!ledBlueState)
   {
      ledBlueState = 1;
      ledRedState = 0;      
   }
   else
   {
      ledBlueState = 0;
      ledRedState = 1;
   }

   gpio_set_value(ledBluePin, ledBlueState);
   gpio_set_value(ledRedPin, ledRedState);

   schedule_delayed_work(&my_work, 1*HZ);
}





///////////////////////////////////////////////
//init function
static int __init blink2_init(void)
{
   printk(KERN_INFO "blink2_init\n");
 
   //set up pins 8-14 and 8-17 (GPIO26 and 27) and set initial
   //state to off
   ledBlueState = 0;
   ledRedState = 0;

   gpio_request(ledBluePin, "label_gpio_pin_26_blue");
   gpio_request(ledRedPin, "label_gpio_pin_27_red");

   gpio_direction_output(ledBluePin, ledBlueState);   // set to output and intial state
   gpio_direction_output(ledRedPin, ledRedState);   // set to output and intial state

   gpio_set_value(ledBluePin, ledBlueState);
   gpio_set_value(ledRedPin, ledRedState);

   //export pin to sys/class/gpio.  Pass False to 
   //prevent the pin direction from being changed
   gpio_export(ledBluePin, false);             
   gpio_export(ledRedPin, false);             

   //init work
   schedule_delayed_work(&my_work, 1*HZ);

   return 0;
}


///////////////////////////////////////////////
//cleanup function
//
static void __exit blink2_exit(void)
{
   printk(KERN_INFO "blink2_exit\n");

   gpio_set_value(ledBluePin, 0);              // Turn the LED off, makes it clear the device was unloaded
   gpio_unexport(ledBluePin);                  // remove ledPin from sys/class/gpio
   gpio_free(ledBluePin);                      // Free the LED GPIO

   gpio_set_value(ledRedPin, 0);              // Turn the LED off, makes it clear the device was unloaded
   gpio_unexport(ledRedPin);                  // remove ledPin from sys/class/gpio
   gpio_free(ledRedPin);                      // Free the LED GPIO

   //remove all items associated with the workqueue
   cancel_delayed_work(&my_work);
   flush_scheduled_work();
}

module_init(blink2_init);
module_exit(blink2_exit);
