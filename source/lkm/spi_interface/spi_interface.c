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


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("Simple SPI Interface");
MODULE_VERSION("0.1");



//magic file path for read/write
static char* spiPath = "/dev/spidev1.0";
static struct file *fp;

//chip select - controlled using normal io
//rather than spi driver.  this is so that 
//we can do a write followed by a read without
//toggling cs pin, and use existing file system
//in /dev/spidev1.0
//
static int spi_cs_pin = 5;
static int spi_cs_high = 1;
static int spi_cs_low = 0;


//function prototypes
static struct file *file_open(const char *path, int flags, int rights);
static void file_close(struct file *file);
static int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size);
static int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size);


//spi functions - these functions will be exported
static void spi_select(void);                                  //cs low
static void spi_deselect(void);                                //cs high
static void spi_tx_bytes(uint8_t* txBuf, uint16_t txLen);      //tx without cs control
static void spi_rx_bytes(uint8_t* rxBuf, uint16_t rxLen);      //rx without cs control

//spi functions that include cs control.
static void spi_tx(uint8_t* txBuf, uint16_t txLen);
static void spi_tx_rx(uint8_t* txBuf, uint16_t txLen, uint8_t* rxBuf, uint16_t rxLen);




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

    return 0;
}

static void __exit spi_interface_exit(void)
{
   printk(KERN_EMERG "spi_interface_exit\n");
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






////////////////////////////////////
//selects the device and opens spidev1.0
void spi_select(void)
{
    gpio_set_value(spi_cs_pin, spi_cs_low);
    fp = file_open(spiPath, O_RDWR, S_IRWXG);
}

////////////////////////////////////////
//close the file and delselect the device
void spi_deselect(void)
{
    file_close(fp);
    gpio_set_value(spi_cs_pin, spi_cs_high);
}

void spi_tx_bytes(uint8_t* txBuf, uint16_t txLen)
{
    if (fp != NULL)
        file_write(fp, 0, txBuf, txLen);
}

void spi_rx_bytes(uint8_t* rxBuf, uint16_t rxLen)
{
    if (fp != NULL)
        file_read(fp, 0, rxBuf, rxLen);
}




///////////////////////////////////////////
//SPI Functions - spi_tx
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

///////////////////////////////////////////
//SPI Functions - spi_tx_rx
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


/////////////////////////////////////////
//EXPORT FUNCTIONS
EXPORT_SYMBOL(spi_select);
EXPORT_SYMBOL(spi_deselect);
EXPORT_SYMBOL(spi_tx_bytes);
EXPORT_SYMBOL(spi_rx_bytes);
EXPORT_SYMBOL(spi_tx);
EXPORT_SYMBOL(spi_tx_rx);



module_init(spi_interface_init);
module_exit(spi_interface_exit);
