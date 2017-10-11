////////////////////////////////////////////////
//Simple hello module.
//Consumer Side - 
//Uses exported symbol from another module
//Load the exporter module first before
//loading this module.
//
#include <linux/module.h>
#include <linux/kernel.h>


//extern symbol exported from hello_export
//module.  The export module contains a static
//function called "hello_export".  Define it here
//as extern so we can use it.  The function is 
//simple, returns a count that increments the
//function is called.  So we should be able to open
//this module over and over and see the count go up.
extern int hello_export(void);

//symbol from blink.ko - export_ioDataOutValue
//an exported symbol from /lkm/blink/blink.c
//returns the value of GPIO1_DATA register
//(ie, the status of all the pins that 
//ride on GPIO1.  You can test this by
//installing module pin9_12 and toggling it
//off and on.  The values here should change)
extern int export_ioDataOutValue(void);

static int hello_init(void)
{
	int value, ioDataOutValue;

	value = hello_export();	
	printk(KERN_EMERG "Consumer: Hello World! - Read Counter: %d\n", value);

	//blink.ko in lkm just flashes the 4 user leds, but since there are other
	//triggers to the leds, you should see some pretty random values
	ioDataOutValue = export_ioDataOutValue();
	printk(KERN_EMERG "Consumer: ioDataOutValue from lkm Blink.ko : 0x%08x\n", ioDataOutValue);



	return 0;
}

static void hello_exit(void)
{
	int value, ioDataOutValue;
	value = hello_export();
	printk(KERN_EMERG "Consumer: Goodby World! - Read Counter: %d\n", value);

	//blink.ko in lkm just flashes the 4 user leds, but since there are other
	//triggers to the leds, you should see some pretty random values
	ioDataOutValue = export_ioDataOutValue();
	printk(KERN_EMERG "Consumer: ioDataOutValue from lkm Blink.ko : 0x%08x\n", ioDataOutValue);

}

module_init(hello_init);
module_exit(hello_exit);

