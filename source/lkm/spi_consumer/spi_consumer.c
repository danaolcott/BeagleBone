/*
Dana Olcott
10/12/17

//////////////////////////////////////////////////
//Linux Kernel Module Programming

SPI Consumer Test Module

The purpose of this module is to test the spi_interface
module and it's exported functions.  This module uses the
Microchip 25AA640A/25LC640A Serial EEPROM IC for 
testing the read/write functions.  

Access the exported functions from spi_interface
by loading the module, running the config program
spi_config to configure the spi pins.  

Add the exported functions by declaring them here
as extern.  

Makefile:  Important:  
Add the following line in the make file to add the
extra symbols.  It does not really matter if you
put the symbols in a .h, declare as extern in the exporter
module, etc.

obj-m:=spi_consumer.o

KBUILD_EXTRA_SYMBOLS+= /home/debian/lkm/spi_interface/Module.symvers

all:
..
..
..

For this test, use a simple workqueue on a timer
to enable write, read the status, disable write, 
and read the status.  The result should be bit 1
on the read byte toggling high/low

Note:
It is assumed were using the following 
pre-configured SPI pins:
P9-17, P9-18, P9-21, P9-22

*/

#include <linux/init.h>       //entry/exit
#include <linux/module.h>     
#include <linux/kernel.h>

#include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("Simple SPI Consumer Test");
MODULE_VERSION("0.1");

//////////////////////////////////////////////////
//Exported functions to be declared as extern
//here (defined in spi_interface lkm module):
//
extern void spi_select(void);                                  //cs low
extern void spi_deselect(void);                                //cs high
extern void spi_tx_bytes(uint8_t* txBuf, uint16_t txLen);      //tx without cs control
extern void spi_rx_bytes(uint8_t* rxBuf, uint16_t rxLen);      //rx without cs control
extern void spi_tx(uint8_t* txBuf, uint16_t txLen);
extern void spi_tx_rx(uint8_t* txBuf, uint16_t txLen, uint8_t* rxBuf, uint16_t rxLen);


/////////////////////////////////////////////
//SPI Consumer Module:
//Use Microchip 25a.... something spi eeprom ic
//defines for eeprom read/write
#define EEPROM_READ            0x03        //read data
#define EEPROM_WRITE           0x02        //write data
#define EEPROM_WRDI            0x04        //reset write enable latch
#define EEPROM_WREN            0x06        //set the write enable latch
#define EEPROM_RDSR            0x05        //read status reg
#define EEPROM_WRSR            0x01        //write status reg

//test functions to generate spi read/write
//and see a change in the MISO line
static void eeprom_writeEnable(void);
static void eeprom_writeDisable(void);
static uint8_t eeprom_readStatus(void);


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
//Simple test function to verify read/write works
//using the spi_interface.ko exported functions.
//Watch /var/log/kern.log tail to see the output
//status bit 1 toggle high and low
//
static void my_work_function(struct work_struct *unused)
{
    uint8_t status;

    eeprom_writeEnable();
    status = eeprom_readStatus();
    printk(KERN_INFO "Work - Enable - Status: 0x%02x\n", status);

    eeprom_writeDisable();
    status = eeprom_readStatus();
    printk(KERN_INFO "Work - Disable - Status: 0x%02x\n", status);

    schedule_delayed_work(&my_work, 1*HZ);
}



////////////////////////////////////////////
//init function
static int __init spi_consumer_init(void)
{
    printk(KERN_EMERG "spi_consumer_init\n");

    schedule_delayed_work(&my_work, 1*HZ);

    return 0;
}

static void __exit spi_consumer_exit(void)
{
    printk(KERN_EMERG "spi_consumer_exit\n");

    //remove all items associated with the workqueue
    cancel_delayed_work(&my_work);      //job 1
    flush_scheduled_work();
}


////////////////////////////////////
//EEPROM Functions - write enable
//
void eeprom_writeEnable(void)
{
    uint8_t txByte = EEPROM_WREN;
    spi_tx(&txByte, 1);
}

////////////////////////////////////
//EEPROM Functions - write disable
//
void eeprom_writeDisable(void)
{
    uint8_t txByte = EEPROM_WRDI;
    spi_tx(&txByte, 1);
}

////////////////////////////////////
//EEPROM Functions - read the status
//
uint8_t eeprom_readStatus(void)
{
    uint8_t tx[] = {EEPROM_RDSR};
    uint8_t rx[] = {0xFF};

    //send RDSR followed by read 1 byte
    spi_tx_rx(tx, 1, rx, 1);

    return rx[0];
}


module_init(spi_consumer_init);
module_exit(spi_consumer_exit);
