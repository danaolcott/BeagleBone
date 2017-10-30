/*
Simple Button and LED Toggle Example
using kobject.

The purpose of this module is to utilize kobject and
attributes to connect a button to an led.  The button
is on an interrupt.  When the button is pressed the led
toggles.  The led represents a toggle switch, identified
by button state.  

Attributes include:
number of button presses
switch state - ie, if the led is on or off
irq - for access from userland.

Pinout:
LED      - Pin 9-12 (60)
Button   - Pin 9-27 (115)

The examples in the sysfs folder follow along with examples
posted at http://www.derekmolloy.ie/, linux module programming
manual, and kernel.org examples.
*/

#include <linux/kobject.h>      //kobject
#include <linux/string.h>       //string to int
#include <linux/sysfs.h>        //sysfs class
#include <linux/module.h>
#include <linux/init.h>
#include <linux/stat.h>         //read/write macro defs

#include <linux/gpio.h>         // Required for the GPIO functions
#include <linux/interrupt.h>    //button handler


/////////////////////////////////////////////
//Hardware globals 
//Pin 9-12 (60) and GPIO Pin 9-27 (115)
//
//LED
static unsigned int ledPin = 60;      //pin 9-12
static unsigned int ledState = 0;     //state 0 - off, 1 - on

//Button
static unsigned int buttonPin = 115;
static unsigned int irq;

//function prototypes - button isr
static irq_handler_t buttonHandler(unsigned int irq_number, void *dev_id, struct pt_regs *regs);


//////////////////////////////////////
//Attributes - 
//For each attribute, we have the following:
//static myVariable - name shown as appears in the sys/kernel/myclass 
//variable_show function - called when doing a cat myVariable
//variable_store function - called when echo # > myVariable

////////////////////////////////////////////
//Attribute - presses

//variable presses
static int presses;

//presses_show - called on cat
static ssize_t presses_show(struct kobject *kobj, struct kobj_attribute *attr,
         char *buf)
{
   return sprintf(buf, "%d\n", presses);
}

//presses_store - called on echo
static ssize_t presses_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
   int ret;

   ret = kstrtoint(buf, 10, &presses);
   if (ret < 0)
      return ret;

   return count;
}

//presses attribute
static struct kobj_attribute presses_attribute =
   __ATTR(presses, 0664, presses_show, presses_store);



////////////////////////////////////////////
//Attribute - state

//variable state - on/off, 1/0
static int state;

//state_show - called on cat
static ssize_t state_show(struct kobject *kobj, struct kobj_attribute *attr,
         char *buf)
{
   return sprintf(buf, "%d\n", state);
}

//////////////////////////////////////////
//state_store - called on echo
//Acceptable values:
//0 - led off
//1 - led on
//2 - led toggle
//
static ssize_t state_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret;

    ret = kstrtoint(buf, 10, &state);
    if (ret < 0)
        return ret;

    if (!state)
    {
      ledState = 0;
    }
    else if (state == 1)
    {
      ledState = 1;
    }

    //toggle  
    else if (state == 2)
    {
        if (ledState == 1)
        {
            state = 0;
            ledState = 0;
        }
        else
        {
            state = 1;
            ledState = 1;
        }
    }

    else
    {
        //out of range, set to off
        state = 0;
        ledState = 0;
    }

    gpio_set_value(ledPin, ledState);

    return count;
}



//state attribute
static struct kobj_attribute state_attribute =
   __ATTR(state, 0664, state_show, state_store);



///////////////////////////////////////
//irq attribute
//irq is already defined
//For readonly attributes, even though this function
//is not passed into the macro, it still needs to be define
//or you'll get an error.  The function name needs
//to be parameter_show, where parameter is the variable name
//that will show up in the file system
static ssize_t irq_show(struct kobject *kobj, struct kobj_attribute *attr,
         char *buf)
{
   return sprintf(buf, "%d\n", irq);
}


//read only macro, pass the name of the attribute
//only, .... irq
//Even though the _show function is not passed into the
//macro, it needs to be defined or you'll get an error
//
static struct kobj_attribute irq_attribute = __ATTR_RO(irq);


//Alternative ways of doing readonly - set the mode to
//
//read only - S_IRUGO
//
//Using the same macro as read/write, pass NULL
//for the _store function.  If you call echo # > irq,
//you'll get a permission denied.  Attribute def
//looks like this:
//
//static struct kobj_attribute irq_attribute =
//   __ATTR(irq, S_IRUGO, irq_show, NULL);




////////////////////////////////////////////
//Attribute Group
//An array of attribute pointers
//one entry for each attribute + NULL
//
static struct attribute *attrs[] = {
   &presses_attribute.attr,
   &state_attribute.attr,
   &irq_attribute.attr,
   NULL,
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
   .attrs = attrs,
};


//////////////////////////////////
//kobject - button_kobj
static struct kobject *button_kobj;

static int __init sysfs_button_init(void)
{
    int retval, result;

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
    gpio_direction_input(buttonPin);                    //button input
    gpio_set_debounce(buttonPin, 500);                  //debounce 500ms

    //enable pullups
    //gpio_pullup(buttonPin, 1);
    gpio_pullup(buttonPin, 1);        //1 - enable, 0 - disable
    gpio_export(buttonPin, false);                      //shows up in sys/class/gpio/gpio115

    //set up irq and interrupts for the button
    irq = gpio_to_irq(buttonPin);

    //connect the lin to the handler function, set the interrupt trigger source
    result = request_irq(irq,
                       (irq_handler_t)buttonHandler,
                       IRQF_TRIGGER_FALLING,
                       "button_handler",
                       NULL);

    //////////////////////////////////////////////////
    //make the kobject with name "button" in the sys/kernel
    //folder.  
    //creates in kernel - a folder named "button"
    button_kobj = kobject_create_and_add("button", kernel_kobj);
    if (!button_kobj)
      return -ENOMEM;

    //Connect the attributes to the kobject.  This also
    //creates the files associated with the kobject.  
    //wondering if you can do an open, close, read write??

    retval = sysfs_create_group(button_kobj, &attr_group);
    if (retval)
      kobject_put(button_kobj);

    return retval;
}


static void __exit sysfs_button_exit(void)
{
   kobject_put(button_kobj);        //destroy the object, remove the files

   //remove the led
   gpio_set_value(ledPin, 0);       // Turn the LED off, makes it clear the device was unloaded
   gpio_unexport(ledPin);           // remove ledPin from sys/class/gpio
   gpio_free(ledPin);               // Free the LED GPIO

   //remove the button
   free_irq(irq, NULL);
   gpio_unexport(buttonPin);
   gpio_free(buttonPin);

}




////////////////////////////////////////////////////////////////
//buttonHandler 
//Function Defintions
static irq_handler_t buttonHandler(unsigned int irq_number, void *dev_id, struct pt_regs *regs)
{
   //print something
   printk(KERN_EMERG "Button Handler!!\n");

   //increment the number of button presses
   presses++;

   //state and ledState are redundant
   if (!ledState)
   {
      ledState = 1;
      state = 1;          //file variable
   }
   else
   {
      ledState = 0;
      state = 0;
   }

    gpio_set_value(ledPin, ledState);

    return (irq_handler_t) IRQ_HANDLED;            
}


module_init(sysfs_button_init);
module_exit(sysfs_button_exit);

MODULE_LICENSE("GPL v2");


