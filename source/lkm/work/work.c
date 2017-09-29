////////////////////////////////////////////////
//Simple workqueue module.
//The purpose of this module is to use workqueue
//to run a function on a timer.  
//To use:
//compile and install the module.
//the message is output to the logs
//use tail -f /var/log/kern.log to look at the message
//displayed.
//
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>			//__init and __exit macros
#include <linux/workqueue.h>	//workqueue

///////////////////////////////////////////
//Workqueue
//Method of setting up a delay to run a function.
//note: "my_work" does not need to be declated, it's
//a name.  you'll get an error if you declare it.
//
static void my_work_function(struct work_struct *unused);
static DECLARE_DELAYED_WORK(my_work, my_work_function);

//////////////////////////////////////////
//my_work_function
//callback function for when delayed work
//timesout.  
static void my_work_function(struct work_struct *unused)
{
	printk(KERN_INFO "Work Function Callback\n");

	schedule_delayed_work(&my_work, 2*HZ);
}



int __init hello_init(void)
{
	printk(KERN_INFO "Hello World!\n");

	//init work
	schedule_delayed_work(&my_work, 2*HZ);

	return 0;
}

void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodby World!\n");

	//remove all items associated with the workqueue
	cancel_delayed_work(&my_work);		//job 1
	flush_scheduled_work();

}

module_init(hello_init);
module_exit(hello_exit);

