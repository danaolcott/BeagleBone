//////////////////////////////////////////////////////////
//Loadable Kernel Module Example using the BeagleBone Black
//
//LED0 Device File
//Make a simple device driver that allows user to 
//set, clear, and toggle LED0.  Assume LED pins
//have been set to output prior to loading this device.
//
//msg[] - holding buffer for linux/user space
//msgSize - size of message from cat/echo/read/write, etc
//
#include <linux/init.h>		//__init and __exit macros
#include <linux/module.h>	//required for any module
#include <linux/device.h>	//register device class
#include <linux/kernel.h>	//required for any module
#include <linux/fs.h>		//file stucture location
#include <asm/uaccess.h>	//for put_user
#include <linux/io.h>		//access to hardware registers


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

#define LED0_OFFSET			(21)	//pin53 - GPIO1
#define LED1_OFFSET			(22)	//pin54 - GPIO1
#define LED2_OFFSET			(23)	//pin55 - GPIO1
#define LED3_OFFSET			(24)	//pin56 - GPIO1

/////////////////////////////////////////////
//kernel memory locations
void __iomem* ioDataOutReg;		//read
void __iomem* ioDataInReg;		//read
void __iomem* ioSetReg;			//write - set
void __iomem* ioClearReg;		//write - clear

static int ioDataOutValue;		//read value from reg
static int ioDataInValue;		//read value from reg



/////////////////////////////////////////
//Module Information
MODULE_LICENSE("GPL");		//has to be GPL or else GPL-only error
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("Generic Device");
MODULE_VERSION("0.1");

////////////////////////////////////////
//Prototypes
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 	0
#define DEVICE_NAME	"dled0"	//name as appears in /proc/devices
#define CLASS_NAME	"dled0"	//not sure if this needs to be same/diff from device name
#define BUF_LEN		10			//max message size from the device



////////////////////////////////////////
//Variables
static int Major = 0;		//major device number
static int Device_Open = 0;	//increments/decrements
static char msg[BUF_LEN] = {0};	//buffer for read/write
static int msgSize = 0;		//size message to/from user

static struct class* charClass = NULL;		//for auto create /dev files
static struct device* charDevice = NULL;	//for auto create /dev files

//fops struct - tempplate
static struct file_operations fops = {
	.open = device_open,
	.release = device_release,
	.read = device_read,
	.write = device_write,
};

/////////////////////////////////////////////
//device_init()
//
static int __init device_init(void)
{
	printk(KERN_INFO "dled0 - device init()");
	printk(KERN_INFO "Register Device: %s\n", DEVICE_NAME);

	//register device, 0 = assign device number dynamically
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0)
	{
		printk(KERN_EMERG "Failed to register device, return Major: %d\n", Major);
		return Major;
	}

	//register was a success, print something
	printk(KERN_INFO "Device was registered with the value Major: %d\n", Major);

	/////////////////////////////////////////////////////
	//Register the device class here so you don't have to 
	//do mknod after the module is loaded.  From:
	//derekmolloy.ie/writing-a-linux-kernel-module-part-2...
	//
	//register the device class using the Major return value
	//
	charClass = class_create(THIS_MODULE, CLASS_NAME);

	if (IS_ERR(charClass))
	{
		unregister_chrdev(Major, DEVICE_NAME);
		printk(KERN_EMERG "Failed to register the device class\n");
		return PTR_ERR(charClass);
	}

	printk(KERN_INFO "Device Class Registered OK\n");

	//Register the device driver
	charDevice = device_create(charClass, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);

	if (IS_ERR(charDevice))
	{
		class_destroy(charClass);
		unregister_chrdev(Major, DEVICE_NAME);
		printk(KERN_EMERG "Failed to Create Device\n");
		return PTR_ERR(charDevice);
	}

	printk(KERN_INFO "Device Class Created OK\n");


	//remap the hardware space to kernel space
	ioDataOutReg = ioremap(GPIO1_OUTPUT_REG, 4);	//read
	ioDataInReg = ioremap(GPIO1_INPUT_REG, 4);		//read
	ioSetReg = ioremap(GPIO1_SET_REG, 4);			//write only
	ioClearReg = ioremap(GPIO1_CLEAR_REG, 4);		//write only	

	//read the current values
	ioDataOutValue = ioread32(ioDataOutReg);		//status of output pins
	ioDataInValue = ioread32(ioDataInReg);			//status of input pins

	//init the kernel message buffer with initial state
	memset(msg, 0x00, BUF_LEN);

	if (ioDataOutValue & LED0_BIT)
		msgSize = sprintf(msg, "1");
	else
		msgSize = sprintf(msg, "0");



	return 0;

}

/////////////////////////////////////////////
//device_exit()
//called when module is unloaded
//destroy the device - charDevice
//unrgister the class
//destroy the class
//unregister the driver
//
static void __exit device_exit(void)
{

	printk(KERN_INFO "chardev.ko - device_exit()\n");

	device_destroy(charClass, MKDEV(Major, 0));	//remove the device
	class_unregister(charClass);			//unrgister device class
	class_destroy(charClass);			//destroy class
	unregister_chrdev(Major, DEVICE_NAME);		//unregister the device

	//clear the led and clear up contens of msg
	iowrite32(LED0_BIT, ioClearReg);
	memset(msg, 0x00, BUF_LEN);
	msgSize = sprintf(msg, "0");
}


////////////////////////////////////////////////
//Called when process tries to open the device file
//ie, cat /dev/mydevicefile, or.... echo > /dev/mydevicefile ??
//
//
static int device_open(struct inode *inode, struct file *file)
{
	//there's a better way to detect if it's open or
	//not based on the linux module tutorial
	if (Device_Open)
		return -EBUSY;

	Device_Open++;

	try_module_get(THIS_MODULE);	//similar to a semephore?

	printk(KERN_INFO "device open\n");

	return SUCCESS;		//return 0
}


static int device_release(struct inode *inode, struct file *file)
{
	//decrement usage count, or else after the file
	//has been opened one time, the module will never 
	//go away, or.. get unloaded... or.?  
	printk(KERN_INFO "device release\n\n");
	Device_Open--;
	module_put(THIS_MODULE);
	return 0;
}

/////////////////////////////////////////
//Called when terminal goes: cat /dev/dgpio18
//NOTE:
//Don't reset the buffer counter on a read.  
//The reason is that cat command calls this function
//until it returns 0?... or...  either way, if you
//keep reseting the position, it will call this function
//for ever!!  Instead, reset the counter in open.
//Need a check to make sure msgPosition < BUF_LEN
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
	int error_count = 0;

	printk(KERN_INFO "Device Read Called\n");

	//read the status of the pin, write the appropriate
	//value into msg (kernel space) and transfer to
	//buffer(user space)
	ioDataOutValue = ioread32(ioDataOutReg);

	if (ioDataOutValue & LED0_BIT)
		msgSize = sprintf(msg, "1");
	else
		msgSize = sprintf(msg, "0");

	//transfer data from kernel space to user space 
	//via the api call copy_to_user
	//buffer = user space buffer
	//msg = kernel space buffer
	//msgSize = length of the message (set on write)
	//error_count = number of bytes not transmitted.
	//
	error_count = copy_to_user(buffer, msg, msgSize);

	if (error_count == 0)
	{
		printk(KERN_EMERG "Msg: %s", msg);
		return (msgSize = 0);
	}
	else
	{
		printk(KERN_EMERG "Failed to Copy %d chars from msg[] into buffer\n", error_count);		
		return -EFAULT;
	}
	
}


//////////////////////////////////////////////////////////
//device write.  
//Called when a process goes: echo "hi" > /dev/my_device.
//A few observations:
//Appears that buff is not null terminated when using echo
//Can't access *buff directly.  (ie, print buff[0], etc).  Need
//to pass it through the call "copy_from_user(kernelBuffer, userBuffer, size)"
//where:
//kernelBuffer = global buffer stored here, also accessed from read
//userBuffer = "buff"
//len = size of the message.
//
//NOTE:
//If you try to access the contents of buff directly
//without going through the "copy_from_user", it will
//crash... EVERYTIME.
//
//
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	int error_count;
	memset(msg, 0x00, BUF_LEN);
	printk(KERN_INFO "Device Write Called\n");

	ioDataOutValue = ioread32(ioDataOutReg);

	//transfer buff[0] into msg - user to kernel space
	error_count = copy_from_user(msg, buff, 1);

	if (error_count == 0)
	{
		msgSize = len;
		printk(KERN_EMERG "ECHO OK: %s\n", msg);

		switch(msg[0])
		{
			//clear
			case '0':
				iowrite32(LED0_BIT, ioClearReg);
				break;
			//set
			case '1':
				iowrite32(LED0_BIT, ioSetReg);
				break;
			//toggle
			case '2':
				if (ioDataOutValue & LED0_BIT)
					iowrite32(LED0_BIT, ioClearReg);
				else
					iowrite32(LED0_BIT, ioSetReg);
				break;
		}
	}	


	return len;
}


module_init(device_init);
module_exit(device_exit);
