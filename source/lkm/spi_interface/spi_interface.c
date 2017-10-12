/*
//////////////////////////////////////////////////
//Linux Kernel Module Programming

SPI Interface using SPIDEV 1.0

The purpose of this module is to create
an interface with the existing SPIDEV 1.0
on the BeagleBoneBlack.  

Assume the SPIDEV is configured and the pins
are functioning.  Uses the following pins:
P9-17, P9-18, P9-21, P9-22

I have no idea is this is going to work, but
the idea is to create the read/write functions
for passing byte, arrays, write followed by 
read etc.  Those functions would then be exported
for other modules to use.

We're not creating a new device driver, only
a ways for others to access the existing driver functions

Access to the file system will be similar to how 
it's described here:

https://stackoverflow.com/questions/1184274/how-to-read-write-files-within-a-linux-kernel-module

mode flag:
 S_IRWXG  00070 group has read, write, and execute permission

*/

#include <linux/init.h>       //entry/exit
#include <linux/module.h>     
#include <linux/kernel.h>


#include <linux/gpio.h>           //confingure CS pin normal gpio
#include <linux/file.h>             //file system
#include <linux/fs.h>             //file system
#include <asm/uaccess.h>            //get_fs, set_fs, ...etc - file system

//do work on a timer
#include <linux/workqueue.h>  //workqueue



//defines for eeprom read/write
#define EE_READ      0x03    //read data
#define EE_WRITE     0x02    //write data
#define EE_WRDI      0x04    //reset write enable latch
#define EE_WREN      0x06    //set the write enable latch
#define EE_RDSR      0x05    //read status reg
#define EE_WRSR      0x01    //write status reg



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("Simple SPI Interface");
MODULE_VERSION("0.1");

//magic file path for read/write
static char* spiPath = "/dev/spidev1.0";
static struct file *fp;

static int spi_cs_pin = 5;
static int spi_cs_high = 1;
static int spi_cs_low = 0;


//function prototypes
static struct file *file_open(const char *path, int flags, int rights);
static void file_close(struct file *file);
static int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size);
static int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size);

//spi functions - these functions will be exported
void spi_tx(uint8_t* txBuf, uint16_t txLen);
void spi_tx_rx(uint8_t* txBuf, uint16_t txLen, uint8_t* rxBuf, uint16_t rxLen);

//EEPROM functions
void eeprom_writeEnable(void);
void eeprom_writeDisable(void);
uint8_t eeprom_readStatus(void); 


///work functions
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
    uint8_t status;
    //disable the write latch
    eeprom_writeDisable();
    status = eeprom_readStatus();
    printk(KERN_EMERG "Write Disable: 0x%02x\n", status);

    eeprom_writeEnable();
    status = eeprom_readStatus();
    printk(KERN_EMERG "Write Enable: 0x%02x\n", status);

    //resched the work to continue the cycle
    schedule_delayed_work(&my_work, 1*HZ);
}



////////////////////////////////////////////
//init function
//assumes SPI pins are configured as spi
//and mode, bits, speed are set.
//Just because, configure CS pin as output
//and set high
static int __init spi_interface_init(void)
{
    printk(KERN_EMERG "spi_interface_init\n");

    //Configure CS pin
    gpio_direction_output(spi_cs_pin, spi_cs_high);   // ste to output and intial state
    gpio_set_value(spi_cs_pin, spi_cs_high);          // Not required as set by line above (here for reference)

    //start the work timer
    schedule_delayed_work(&my_work, 1*HZ);

    return 0;
}

static void __exit spi_interface_exit(void)
{
   printk(KERN_EMERG "spi_interface_exit\n");

   //remove all items associated with the workqueue
   cancel_delayed_work(&my_work);
   flush_scheduled_work();

}







///////////////////////////////////////////
//File Operations - file open
struct file *file_open(const char *path, int flags, int rights) 
{
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

///////////////////////////////////////////
//File Operations - file close
void file_close(struct file *file) 
{
    filp_close(file, NULL);
}

///////////////////////////////////////////
//File Operations - file read
int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}   

///////////////////////////////////////////
//File Operations - file write
int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}


void spi_tx(uint8_t* txBuf, uint16_t txLen)
{
    //cs pin
    gpio_set_value(spi_cs_pin, spi_cs_low);

    fp = file_open(spiPath, O_RDWR, S_IRWXG);
    file_write(fp, 0, txBuf, txLen);
    file_close(fp);

    //cs pin
    gpio_set_value(spi_cs_pin, spi_cs_high);
}

void spi_tx_rx(uint8_t* txBuf, uint16_t txLen, uint8_t* rxBuf, uint16_t rxLen)
{
    //cs pin
    gpio_set_value(spi_cs_pin, spi_cs_low);

    fp = file_open(spiPath, O_RDWR, S_IRWXG);
    file_write(fp, 0, txBuf, txLen);
    file_read(fp, 0, rxBuf, rxLen);
    file_close(fp);

    //cs pin
    gpio_set_value(spi_cs_pin, spi_cs_high);
}

void eeprom_writeEnable()
{
    uint8_t txByte = EE_WREN;
    spi_tx(&txByte, 1);    
}

void eeprom_writeDisable()
{
    uint8_t txByte = EE_WRDI;
    spi_tx(&txByte, 1);
}


uint8_t eeprom_readStatus(void)
{
    uint8_t tx[] = {EE_RDSR};
    uint8_t rx[] = {0xFF};

    //send RDSR followed by read 1 byte
    spi_tx_rx(tx, 1, rx, 1);

    return rx[0];
}

module_init(spi_interface_init);
module_exit(spi_interface_exit);
