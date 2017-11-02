////////////////////////////////////////////////
//10/31/17
//
//Backpack Loadable Kernel Module
//The purpose of this module is initialize all
//modules for use by the backpack pca in an orderly
//manor, followed by launching a FreeRTOS demo.  
//
//Make upcalls to the following:
//
//spi_config - configure the spi driver for the eeprom
//spi_interface - start the spi interface for the eeprom
//lcd_interface - start the lcd interface for writing to lcd
//sysfs_eeprom - file system for eeprom
//sysfs_lcd - file system for eeprom
//button??
//
//Finally, run FreeRTOS demo
//
//start each of these in a workqueue starting 
//after about 10 seconds
//
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>			//__init and __exit macros
#include <linux/workqueue.h>	//workqueue
#include <linux/kmod.h>		//user space function call prototypes



/////////////////////////////////////////
//Module Information
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("Backpack Board Init Module");
MODULE_VERSION("0.1");


/////////////////////////////////////////////
//globals
static int launchCounter = 0x00;



///////////////////////////////////////////
//functions to run
static int run_eeprom_init(void);		//called from workqueue
static int run_lcd_init(void);
static int run_button_led_init(void);
static int run_user_progs(void);


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

	if (launchCounter == 8)
	{
		//eeprom
		printk(KERN_EMERG "Work Function: Start EEPROM Components: %d\n", launchCounter);
		run_eeprom_init();
	}

	else if (launchCounter == 16)
	{
		//lcd
		printk(KERN_EMERG "Work Function: Start LCD Components: %d\n", launchCounter);
		run_lcd_init();		
	}

	else if (launchCounter == 24)
	{
		//buttons and leds
		printk(KERN_EMERG "Work Function: Start Buttons and LEDs: %d\n", launchCounter);
		run_button_led_init();		
	}

	else if (launchCounter == 32)
	{
		//user space demo program that uses the drivers loaded above
		printk(KERN_EMERG "Work Function: Start User RTOS: %d\n", launchCounter);
		run_user_progs();
	}

	launchCounter++;

	if (launchCounter < 35)
		schedule_delayed_work(&my_work, 1*HZ);	
}



int __init backpack_init(void)
{
	printk(KERN_EMERG "backpack_init\n");

	launchCounter = 0;

	//init work
	schedule_delayed_work(&my_work, 1*HZ);

	return 0;
}

void __exit backpack_exit(void)
{
	printk(KERN_EMERG "backpack_exit\n");

	launchCounter = 0x00;

	cancel_delayed_work(&my_work);
	flush_scheduled_work();

}





int run_eeprom_init(void)
{
    int ret;

    //dummy args arg1, arg2, NULL
    char *argv[] = { "/home/debian/cprogs/backpack/eeprom_init", "arg1", NULL};
    static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

    printk(KERN_EMERG "UPCALL: run_eeprom_init\n");

    //envp - path to excutables that acutally run the program, not 
    //the program itself.
    ret = call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC);

    if (ret != 0)
        printk("Error: run_eeprom_init: %d\n", ret);
    else
        printk("Success: run_eeprom_init\n");

      return ret;
}


int run_lcd_init(void)
{
    int ret;

    //dummy args arg1, arg2, NULL
    char *argv[] = { "/home/debian/cprogs/backpack/lcd_init", "arg1", NULL};
    static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

    printk(KERN_EMERG "UPCALL: run_lcd_init\n");

    //envp - path to excutables that acutally run the program, not 
    //the program itself.
    ret = call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC);

    if (ret != 0)
        printk("Error: run_lcd_init: %d\n", ret);
    else
        printk("Success: run_lcd_init\n");

      return ret;
}

int run_button_led_init(void)
{
    int ret;

    //dummy args arg1, arg2, NULL
    char *argv[] = { "/home/debian/cprogs/backpack/button_led_init", "arg1", NULL};
    static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

    printk(KERN_EMERG "UPCALL: run_button_led_init\n");

    //envp - path to excutables that acutally run the program, not 
    //the program itself.
    ret = call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC);

    if (ret != 0)
        printk("Error: run_button_led_init: %d\n", ret);
    else
        printk("Success: run_button_led_init\n");

      return ret;
}

//////////////////////////////////////////////
//cprogs - user_progs
//user_progs is an exe file used for starting
//all user programs ran in the demo.
//
int run_user_progs(void)
{
    int ret;

    //dummy args arg1, arg2, NULL
    char *argv[] = { "/home/debian/cprogs/backpack/user_progs", "arg1", NULL};
    static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

    printk(KERN_EMERG "UPCALL: run_user_progs\n");

    //envp - path to excutables that acutally run the program, not 
    //the program itself.
    ret = call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC);

    if (ret != 0)
        printk("Error: run_user_progs: %d\n", ret);
    else
        printk("Success: run_user_progs\n");

      return ret;
}










module_init(backpack_init);
module_exit(backpack_exit);

