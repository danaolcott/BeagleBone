/*
//////////////////////////////////////////////////
//Linux Kernel Module Programming

Extension of the simple button/led example where
now, use all 3 user buttons and two leds on the
backpack board.  The concept is similar in that
each button flashes an led.  One of the buttons will
toggle both leds.  

Note:  117 used to be button right, but there is a 
conflict with external osc output, so that didn't work.
moved to GPIO61

Pins - 
LEDs     - Pin 8-14 (GPIO26) and 8-17 (GPIO27)
Button   - Pin 9-30 (112), 9-27 (115), 8-26 (61)

*/

#include <linux/init.h>       //entry/exit
#include <linux/module.h>     
#include <linux/kernel.h>
#include <linux/gpio.h>       // Required for the GPIO functions
#include <linux/interrupt.h>  //button handler
#include <linux/workqueue.h>  //for timer

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("Three Buttons and LEDs Example");
MODULE_VERSION("0.1");

/////////////////////////////////////////////
//Globals for tracking state of the LEDs 
//Pin 8-14(26) and 8-17(27) and buttons
//(pins 112, 115, 61)
//
//LEDs
static unsigned int ledBluePin = 26;
static unsigned int ledBlueState = 0;     //state 0 - off, 1 - on
static unsigned int numBlueFlashes = 0;   //number of workqueue passes on a single press

static unsigned int ledRedPin = 27;
static unsigned int ledRedState = 0;     //state 0 - off, 1 - on
static unsigned int numRedFlashes = 0;   //number of workqueue passes on a single press

//Button
static unsigned int buttonLeftPin = 112;     //left
static unsigned int irqButtonLeft;
static unsigned int buttonCenterPin = 115;   //center
static unsigned int irqButtonCenter;
static unsigned int buttonRightPin = 61;    //right
static unsigned int irqButtonRight;


//function prototypes - workqueue - blue led
static void work_function_blue(struct work_struct *unused);
static DECLARE_DELAYED_WORK(work_blue, work_function_blue);

//function prototypes - workqueue - red led
static void work_function_red(struct work_struct *unused);
static DECLARE_DELAYED_WORK(work_red, work_function_red);


//function prototypes - button handler functions
static irq_handler_t buttonLeftHandler(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t buttonCenterHandler(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t buttonRightHandler(unsigned int irq, void *dev_id, struct pt_regs *regs);



///////////////////////////////////////////////
//button3_init function
//Called when module is loaded
static int __init button3_init(void)
{
   int result;

   printk(KERN_INFO "button3_init\n");
 
   //set up pin 8-14 (GPIO26) and set the initial state to off
   ledBlueState = 0;
   gpio_request(ledBluePin, "LED_PIN_GPIO_8_14");   //writes to label 
   gpio_direction_output(ledBluePin, ledBlueState);   // set to output and intial state
   gpio_set_value(ledBluePin, ledBlueState);
   gpio_export(ledBluePin, false);      //export pin to sys/class/gpio, False to prevent direction change

   //set up pin 8-17 (GPIO27) and set the initial state to off
   ledRedState = 0;
   gpio_request(ledRedPin, "LED_PIN_GPIO_8_17");   //writes to label 
   gpio_direction_output(ledRedPin, ledRedState);   // set to output and intial state
   gpio_set_value(ledRedPin, ledRedState);
   gpio_export(ledRedPin, false);      //export pin to sys/class/gpio, False to prevent direction change

   //buttons - set up pin9_30 (GPIO112)
   gpio_request(buttonLeftPin, "Button_Left_PIN_GPIO_9_30");   //writes to label sys/class/gpio/gpio112/label
   gpio_direction_input(buttonLeftPin);                        //button input
   gpio_set_debounce(buttonLeftPin, 500);                      //debounce 500ms
   gpio_export(buttonLeftPin, false);                          //shows up in sys/class/gpio/gpio112

   //set up irq and interrupts for the button
   irqButtonLeft = gpio_to_irq(buttonLeftPin);

   //connect the line to the handler function, trigger on
   //falling since we have a pullup resistor
   //
   result = request_irq(irqButtonLeft,
                        (irq_handler_t)buttonLeftHandler,
                        IRQF_TRIGGER_FALLING,
                        "button_left_handler",
                        NULL);


   //buttons - set up pin9_27 (GPIO115)
   gpio_request(buttonCenterPin, "Button_Left_PIN_GPIO_9_27");   //writes to label sys/class/gpio/gpio115/label
   gpio_direction_input(buttonCenterPin);                        //button input
   gpio_set_debounce(buttonCenterPin, 500);                      //debounce 500ms
   gpio_export(buttonCenterPin, false);                          //shows up in sys/class/gpio/gpio115

   //set up irq and interrupts for the button
   irqButtonCenter = gpio_to_irq(buttonCenterPin);

   //connect the line to the handler function, trigger on
   //falling since we have a pullup resistor
   //
   result = request_irq(irqButtonCenter,
                        (irq_handler_t)buttonCenterHandler,
                        IRQF_TRIGGER_FALLING,
                        "button_center_handler",
                        NULL);


   //buttons - set up pin8_26 (GPIO61)
   gpio_request(buttonRightPin, "Button_Right_PIN_GPIO_8_26");      //writes to label sys/class/gpio/gpio61/label
   gpio_direction_input(buttonRightPin);                         //button input
   gpio_set_debounce(buttonRightPin, 500);                       //debounce 500ms
   gpio_export(buttonRightPin, false);                           //shows up in sys/class/gpio/gpio61

   //set up irq and interrupts for the button
   irqButtonRight = gpio_to_irq(buttonRightPin);

   //connect the line to the handler function, trigger on
   //falling since we have a pullup resistor
   //
   result = request_irq(irqButtonRight,
                        (irq_handler_t)buttonRightHandler,
                        IRQF_TRIGGER_FALLING,
                        "button_right_handler",
                        NULL);

   return result;
}


///////////////////////////////////////////////
//cleanup function
//called when module is unloaded
static void __exit button3_exit(void)
{
   printk(KERN_INFO "button3_exit\n");

   //remove the ledBlue
   gpio_set_value(ledBluePin, 0);              // Turn the LED off, makes it clear the device was unloaded
   gpio_unexport(ledBluePin);                  // remove ledPin from sys/class/gpio
   gpio_free(ledBluePin);                      // Free the LED GPIO

   //remove the ledRed
   gpio_set_value(ledRedPin, 0);              // Turn the LED off, makes it clear the device was unloaded
   gpio_unexport(ledRedPin);                  // remove ledPin from sys/class/gpio
   gpio_free(ledRedPin);                      // Free the LED GPIO

   //remove buttonLeft
   free_irq(irqButtonLeft, NULL);
   gpio_unexport(buttonLeftPin);
   gpio_free(buttonLeftPin);

   //remove buttonCenter
   free_irq(irqButtonCenter, NULL);
   gpio_unexport(buttonCenterPin);
   gpio_free(buttonCenterPin);

   //remove buttonRight
   free_irq(irqButtonRight, NULL);
   gpio_unexport(buttonRightPin);
   gpio_free(buttonRightPin);

   //remove all items associated with the workqueue
   cancel_delayed_work(&work_blue);
   cancel_delayed_work(&work_red);
   flush_scheduled_work();

}



/////////////////////////////////////////////////
//work_function_blue
//Function called when delayed work timesout
//Toggle ledBluePin
//
static void work_function_blue(struct work_struct *unused)
{
   if (numBlueFlashes > 0)
   {
      numBlueFlashes--;
      if (!ledBlueState)
         ledBlueState = 1;
      else
         ledBlueState = 0;

      gpio_set_value(ledBluePin, ledBlueState);
      schedule_delayed_work(&work_blue, 1*HZ / 10);
   }
}


/////////////////////////////////////////////////
//work_function_red
//Function called when delayed work timesout
//Toggle ledRedPin
//
static void work_function_red(struct work_struct *unused)
{
   if (numRedFlashes > 0)
   {
      numRedFlashes--;
      if (!ledRedState)
         ledRedState = 1;
      else
         ledRedState = 0;

      gpio_set_value(ledRedPin, ledRedState);
      schedule_delayed_work(&work_red, 1*HZ / 10);
   }
}




////////////////////////////////////////////////////////////////
//buttonLeftHandler 
//Function Defintions
static irq_handler_t buttonLeftHandler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
   //print something
   printk(KERN_EMERG "Button Left Handler!!\n");

   //if done flashing blue led, reset the flash
   //counter and post delayed work  
   if (!numBlueFlashes)
   {
      //start the work
      numBlueFlashes = 10;      //reset
      //init work
      schedule_delayed_work(&work_blue, 1*HZ / 10);      
   }

   return (irq_handler_t) IRQ_HANDLED;            
}


////////////////////////////////////////////////////////////////
//buttonCenterHandler 
//Function Defintions
static irq_handler_t buttonCenterHandler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
   //print something
   printk(KERN_EMERG "Button Center Handler!!\n");

   //if done flashing blue red, reset the flash
   //counter and post delayed work  
   if (!numRedFlashes)
   {
      //start the work
      numRedFlashes = 10;      //reset
      //init work
      schedule_delayed_work(&work_red, 1*HZ / 10);      
   }

   return (irq_handler_t) IRQ_HANDLED;            
}


////////////////////////////////////////////////////////////////
//buttonRightHandler 
//Function Defintions
//simply toggle the red and blue leds
//
static irq_handler_t buttonRightHandler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
   //print something
   printk(KERN_EMERG "Button Right Handler!!\n");

   //toggle the led states of blue and red leds
   if (!ledRedState)
      ledRedState = 1;
   else
      ledRedState = 0;

   if (!ledBlueState)
      ledBlueState = 1;
   else
      ledBlueState = 0;

   gpio_set_value(ledBluePin, ledBlueState);
   gpio_set_value(ledRedPin, ledRedState);

   return (irq_handler_t) IRQ_HANDLED;   
}


module_init(button3_init);
module_exit(button3_exit);
