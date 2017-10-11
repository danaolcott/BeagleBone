/*
//////////////////////////////////////////////////
//Linux Kernel Module Programming

Simple Button Example with Interrupt

The purpose of this module is to use /linux/gpio.h
to toggle an led when a button is pressed.  Pressing
the button triggers the interrpt that runs a functinon.
The function resets a static counter to post delayed work
to another function.

Pins - 
LED      - Pin 9-12 (60)
Button   - Pin 9-27 (115)

The examples in the sysfs folder follow along with tutorials
posted at  http://www.derekmolloy.ie/ and use the linux/gpio.h
functions.  Also info from linux module programming manual

*/

#include <linux/init.h>       //entry/exit
#include <linux/module.h>     
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/interrupt.h>  //button handler
#include <linux/workqueue.h>  //for timer

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("Simple LED Toggle Example");
MODULE_VERSION("0.1");

/////////////////////////////////////////////
//Globals for tracking state of the GPIO 
//Pin 9-12 (60) and GPIO Pin 9-27 (115)
//
//LED
static unsigned int ledPin = 60;      //pin 9-12
static unsigned int ledState = 0;     //state 0 - off, 1 - on
static unsigned int numFlashes = 0;   //number of workqueue passes on a single press

//Button
static unsigned int buttonPin = 115;
static unsigned int irqNumber;

//function prototypes - workqueue
static void my_work_function(struct work_struct *unused);
static DECLARE_DELAYED_WORK(my_work, my_work_function);

//function prototypes - button isr
static irq_handler_t buttonHandler(unsigned int irq, void *dev_id, struct pt_regs *regs);



///////////////////////////////////////////////
//button_init function
//Called when module is loaded
static int __init button_init(void)
{
   int result;

   printk(KERN_INFO "button_init\n");
 
   //set up pin 9 - 12 (GPIO60) and set the initial state to off
   ledState = 0;
   gpio_request(ledPin, "LED_PIN_GPIO_9_12");   //writes to label 
   gpio_direction_output(ledPin, ledState);   // ste to output and intial state
   gpio_set_value(ledPin, ledState);          // Not required as set by line above (here for reference)

   //export pin to sys/class/gpio.  Pass False to 
   //prevent the pin direction from being changed
   gpio_export(ledPin, false);             

   //button - set up pin9_27 (GPIO115)
   gpio_request(buttonPin, "Button_PIN_GPIO_9_27");    //writes to label sys/class/gpio/gpio115/label
   gpio_direction_input(buttonPin);                   //button input
   gpio_set_debounce(buttonPin, 500);                 //debounce 500ms
   gpio_export(buttonPin, false);                  //shows up in sys/class/gpio/gpio115

   //set up irq and interrupts for the button
   irqNumber = gpio_to_irq(buttonPin);

   //connect the lin to the handler function, set the interrupt trigger source
   result = request_irq(irqNumber,
                        (irq_handler_t)buttonHandler,
                        IRQF_TRIGGER_RISING,
                        "button_handler",
                        NULL);

   return result;
}


///////////////////////////////////////////////
//cleanup function
//called when module is unloaded
static void __exit button_exit(void)
{
   printk(KERN_INFO "button_exit\n");

   //remove the led
   gpio_set_value(ledPin, 0);              // Turn the LED off, makes it clear the device was unloaded
   gpio_unexport(ledPin);                  // remove ledPin from sys/class/gpio
   gpio_free(ledPin);                      // Free the LED GPIO

   //remove the button
   free_irq(irqNumber, NULL);
   gpio_unexport(buttonPin);
   gpio_free(buttonPin);

   //remove all items associated with the workqueue
   cancel_delayed_work(&my_work);
   flush_scheduled_work();
}



/////////////////////////////////////////////////
//my_work_function
//Function called when delayed work timesout
//Toggle ledPin
//
static void my_work_function(struct work_struct *unused)
{
   if (numFlashes > 0)
   {
      numFlashes--;
      if (!ledState)
         ledState = 1;
      else
         ledState = 0;

      gpio_set_value(ledPin, ledState);
      schedule_delayed_work(&my_work, 1*HZ / 10);
   }

   else
   {
      //do nothing.
   }
}





////////////////////////////////////////////////////////////////
//buttonHandler 
//Function Defintions
static irq_handler_t buttonHandler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
   //print something
   printk(KERN_EMERG "Button Handler!!\n");

   //if done flashing, reset the flash
   //counter and post delayed work   
   if (!numFlashes)
   {
      //start the work
      numFlashes = 10;      //reset
      //init work
      schedule_delayed_work(&my_work, 1*HZ / 10);      
   }

   return (irq_handler_t) IRQ_HANDLED;            
}


module_init(button_init);
module_exit(button_exit);
