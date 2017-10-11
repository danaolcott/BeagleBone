////////////////////////////////////////////////
//Simple hello module.
//
#include <linux/module.h>
#include <linux/kernel.h>

static int counter = 0;

static int hello_init(void)
{
	printk(KERN_EMERG "Hello World!\n");
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_EMERG "Goodby World!\n");
}


////////////////////////////////////
//exported symbol function - hello_export
static int hello_export(void)
{
	counter++;
	printk(KERN_EMERG "Message from hello_export function / module\n");
	return counter;
}


//export the function so anyone can use it
EXPORT_SYMBOL(hello_export);

module_init(hello_init);
module_exit(hello_exit);

