//////////////////////////////////////////////////////////
//Loadable Kernel Module Example using the BeagleBone Black
//
//Generic Device File
//Make a simple device driver that allows you to echo and cat
//data to and from kernel space and user space.
//
//msg[] is the holding buffer for kernel space data
//size_of_msg is the length of the message, set on write
//and cleared on read.
//
//

#include <linux/init.h>		//__init and __exit macros
#include <linux/module.h>	//required for any module
#include <linux/device.h>	//register device class
#include <linux/kernel.h>	//required for any module
#include <linux/fs.h>		//file stucture location
#include <asm/uaccess.h>	//for put_user


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
#define DEVICE_NAME	"chardev"	//name as appears in /proc/devices
#define CLASS_NAME	"chardev"	//not sure if this needs to be same/diff from device name
#define BUF_LEN		256			//max message size from the device



////////////////////////////////////////
//Variables
static int Major = 0;		//major device number
static int Device_Open = 0;	//increments/decrements
static char msg[BUF_LEN] = {0};	//buffer for read/write
static int size_of_msg = 0;
static char* msgPtr = msg;

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
	printk(KERN_INFO "chardev - device init()");
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

	//init the kernel message buffer with intial message
	//and set the size of the message variable size_of_message
	memset(msg, 0x00, BUF_LEN);
	size_of_msg = sprintf(msg, "Kernel Space Message\n");

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

	//clean up the message buffer
	memset(msg, 0x00, BUF_LEN);
	size_of_msg = sprintf(msg, "Kernel Space Message");
}


////////////////////////////////////////////////
//Called when process tries to open the device file,
//ie, cat /dev/mydevicefile, or.... echo > /dev/mydevicefile ??
//
//Funny, this function is not called, at least the printk is not
//printed.  The buffer gets loaded so it's got to be called from
//somewhere, but I don't see the -device open message
static int device_open(struct inode *inode, struct file *file)
{
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
	int bytes_read = 0x00;

	printk(KERN_INFO "Device Read Called\n");

	//transfer data from kernel space to user space 
	//via the api call copy_to_user
	//buffer = user space buffer
	//msg = kernel space buffer
	//size_of_message = length of the message (set on write)
	//error_count = number of bytes not transmitted.
	//
	if (*msgPtr == 0)
	{
		msgPtr = msg;
		return 0;
	}

	while (length && *msgPtr)
	{
		put_user(*(msgPtr++), buffer++);
		length--;
		bytes_read++;		
	}

	return bytes_read;

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
//size = size of the message.
//
//msgPtr - position for readback when calling cat
//or device read.  increments on read until it 
//hits 0x00 at which time read returns 0
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

	//copy contents of buff into kernel space memory
	error_count = copy_from_user(msg, buff, len);

	if (error_count == 0)
	{
		size_of_msg = len;
		msgPtr = msg;			//set the ptr to front of message
		printk(KERN_EMERG "ECHO OK: %s\n", msg);
	}

	return len;
}


module_init(device_init);
module_exit(device_exit);
