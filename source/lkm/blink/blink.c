////////////////////////////////////////////////
//Simple Blink Module.
//9/28/17
//Dana Olcott
//
//The purpose of this module is to toggle the user
//leds on a timer using a workqueue.  This creates
//an irregular pattern since there are other triggers
//that toggle the leds.
//
//The workqueue will call a function over and over.
//The function will read a register that contains
//GPIO1 pin data.  Writes to set and clear registers
//are used to toggle the leds.  
//
//tail -f /var/log/kern.log shows the messages
//from the kernel module.
//
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>			//__init and __exit macros
#include <linux/workqueue.h>	//workqueue
#include <linux/io.h>			//gpio register access


//////////////////////////////////////////
//Register definitions
//GPIO Base Addresses
#define GPIO0_ADDR 0x44E07000		//GPIO0 base address
#define GPIO1_ADDR 0x4804C000		//GPIO1 base address
#define GPIO2_ADDR 0x481AC000		//GPIO2 base address
#define GPIO3_ADDR 0x481AF000		//GPIO3 base address

////////////////////////////////////////////////////
//GPIO Offsets - Apply to base for a specific IO
//This is a partial list, not sure if DATAIN is corrrect
//or what it's used for.
//use DATAOUT, SET, and CLEAR offsets for capturing the 
//status of GPIOX, set pin, and clearing pins.
//
#define OE_ADDR 				0x134	//output enable?
#define GPIO_DATAOUT 			0x13C	//pin states for output pins
#define GPIO_DATAIN 			0x138	//??
#define GPIO_CLEARDATAOUT		0x190	//write to clear a pin
#define GPIO_SETDATAOUT			0x194	//write to set a pin


//////////////////////////////////////////
//Control Registers
//
#define GPIO1_OUTPUT_REG		(GPIO1_ADDR | GPIO_DATAOUT)			//read
#define GPIO1_INPUT_REG			(GPIO1_ADDR | GPIO_DATAIN)			//read
#define GPIO1_SET_REG			(GPIO1_ADDR | GPIO_SETDATAOUT)		//write
#define GPIO1_CLEAR_REG			(GPIO1_ADDR | GPIO_CLEARDATAOUT)	//write

/////////////////////////////////////////
//LED Bits
#define LED0_BIT			(1 << 21)	//pin53 - GPIO1
#define LED1_BIT			(1 << 22)	//pin54 - GPIO1
#define LED2_BIT			(1 << 23)	//pin55 - GPIO1
#define LED3_BIT			(1 << 24)	//pin56 - GPIO1

/////////////////////////////////////////////
//kernel memory locations
void __iomem* ioDataOutReg;		//read
void __iomem* ioDataInReg;		//read
void __iomem* ioSetReg;			//write - set
void __iomem* ioClearReg;		//write - clear

static int ioDataOutValue;		//read value from reg
static int ioDataInValue;		//read value from reg


///////////////////////////////////////////
//Workqueue
//Method of setting up a delay to run a function.
//note: "my_work" does not need to be declared, it's
//a name.  you'll get an error if you declare it.
//
static void my_work_function(struct work_struct *unused);
static DECLARE_DELAYED_WORK(my_work, my_work_function);

///////////////////////////////////////////////
//my_work_function
//Timeout callback function.  
//Print a message.
//Read the status of the 4 user leds
//Toggle the leds
//output the values to the terminal
//
static void my_work_function(struct work_struct *unused)
{
	printk(KERN_INFO "Work Function Callback\n");

	//read the status
	ioDataOutValue = ioread32(ioDataOutReg);

	//capture bits
	//LED0
	if (ioDataOutValue & LED0_BIT)
		iowrite32(LED0_BIT, ioClearReg);
	else
		iowrite32(LED0_BIT, ioSetReg);

	//LED1
	if (ioDataOutValue & LED1_BIT)
		iowrite32(LED1_BIT, ioClearReg);
	else
		iowrite32(LED1_BIT, ioSetReg);

	//LED2
	if (ioDataOutValue & LED2_BIT)
		iowrite32(LED2_BIT, ioClearReg);
	else
		iowrite32(LED2_BIT, ioSetReg);

	//LED3
	if (ioDataOutValue & LED3_BIT)
		iowrite32(LED3_BIT, ioClearReg);
	else
		iowrite32(LED3_BIT, ioSetReg);

	//values
	ioDataOutValue = ioread32(ioDataOutReg);
	ioDataInValue = ioread32(ioDataInReg);

	//print the final values
	printk(KERN_INFO "DataOutValue: 0x%08x\n", ioDataOutValue);
	printk(KERN_INFO "DataInValue: 0x%08x\n", ioDataInValue);	

	//resched the work to continue the cycle
	schedule_delayed_work(&my_work, 2*HZ);
}



int __init hello_init(void)
{
	printk(KERN_INFO "Hello World!\n");

	//remap the hardware space to kernel space
	ioDataOutReg = ioremap(GPIO1_OUTPUT_REG, 4);	//read
	ioDataInReg = ioremap(GPIO1_INPUT_REG, 4);		//read
	ioSetReg = ioremap(GPIO1_SET_REG, 4);			//write only
	ioClearReg = ioremap(GPIO1_CLEAR_REG, 4);		//write only	

	//read the current values
	ioDataOutValue = ioread32(ioDataOutReg);		//status of output pins
	ioDataInValue = ioread32(ioDataInReg);			//status of input pins

	//print the initial values
	printk(KERN_INFO "DataOutValue: 0x%08x\n", ioDataOutValue);
	printk(KERN_INFO "DataInValue: 0x%08x\n", ioDataInValue);	

	//start the work cycle
	schedule_delayed_work(&my_work, 2*HZ);

	return 0;
}

void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodby World!\n");

	//remove all items associated with the workqueue
	cancel_delayed_work(&my_work);
	flush_scheduled_work();

	//clear all the leds
	iowrite32(LED0_BIT | LED1_BIT | LED2_BIT | LED3_BIT, ioClearReg);
}

module_init(hello_init);
module_exit(hello_exit);

