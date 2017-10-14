/*
//////////////////////////////////////////////////
//Linux Kernel Module Programming

Simple Flashing LED Example using Linux/gpio.h

The purpose of this module is to use /linux/gpio.h
to toggle an led.  The led will toggle using a workqueue
and posting delayed work.

*/

#include <linux/init.h>       //entry/exit
#include <linux/module.h>     
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/workqueue.h>  //for timer

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("Simple LED Toggle Example");
MODULE_VERSION("0.1");



/////////////////////////////////////////////
//Globals for tracking state of the GPIO Pin 9-12
//Located on GPIO1, Pin 28, GPIO Pin 60
//
static unsigned int ledPin = 60;      //pin 9-12
static unsigned int ledState = 0;     //state 0 - off, 1 - on



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
   if (!ledState)
      ledState = 1;
   else
      ledState = 0;

   gpio_set_value(ledPin, ledState);

   schedule_delayed_work(&my_work, 1*HZ);
}





///////////////////////////////////////////////
//init function
static int __init blink2_init(void)
{
   printk(KERN_INFO "blink2_init\n");
 
   //set up pin 9 - 12 and set the initial state to off
   ledState = 0;
   gpio_request(ledPin, "my_label_gpio_pin_60_is_assigned_to_p9_12");
   gpio_direction_output(ledPin, ledState);   // ste to output and intial state
   gpio_set_value(ledPin, ledState);          // Not required as set by line above (here for reference)

   //export pin to sys/class/gpio.  Pass False to 
   //prevent the pin direction from being changed
   gpio_export(ledPin, false);             

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

   gpio_set_value(ledPin, 0);              // Turn the LED off, makes it clear the device was unloaded
   gpio_unexport(ledPin);                  // remove ledPin from sys/class/gpio
   gpio_free(ledPin);                      // Free the LED GPIO

   //remove all items associated with the workqueue
   cancel_delayed_work(&my_work);
   flush_scheduled_work();
}

module_init(blink2_init);
module_exit(blink2_exit);
