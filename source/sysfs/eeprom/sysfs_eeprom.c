////////////////////////////////////////////////////
/*
EEPROM Controller Module for the Microchip
25AA640A/25LC640A Serial EEPROM IC

Attributes
WIP - Write In Progress (0 or 1)
    - Value from IC and Stored

WREN - Write Enable (0 or 1)
    - Value to/from IC and Stored

ADDR - Current Read/Write Address
    - Value stored locally

DATA - Value Stored at the Current Address
    - Value to/from IC and Stored

AUTOINC - Auto increment (0 or 1)
    - auto increment the destination address
      on each write or read. 0 - manual,
      1 - auto increment

*/
///////////////////////////////////////////////////

#include <linux/kobject.h>      //kobject
#include <linux/string.h>       //string to int
#include <linux/sysfs.h>        //sysfs class
#include <linux/module.h>
#include <linux/init.h>
#include <linux/stat.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("EEPROM Controller Module");
MODULE_VERSION("0.1");

//////////////////////////////////////////////////
//Exported functions to be declared as extern
//here (defined in spi_interface lkm module):
//spi_interface.ko needs to be installed (and 
//run spi_config to configure pins, speed, etc)
//prior to installing this module.
//
extern void spi_select(void);                                  //cs low
extern void spi_deselect(void);                                //cs high
extern void spi_tx_bytes(uint8_t* txBuf, uint16_t txLen);      //tx without cs control
extern void spi_rx_bytes(uint8_t* rxBuf, uint16_t rxLen);      //rx without cs control
extern void spi_tx(uint8_t* txBuf, uint16_t txLen);
extern void spi_tx_rx(uint8_t* txBuf, uint16_t txLen, uint8_t* rxBuf, uint16_t rxLen);


/////////////////////////////////////////////
//EEPROM Defines
//See datasheet for 
//25AA640A/25LC640A Serial EEPROM IC
//
#define EEPROM_READ            0x03        //read data
#define EEPROM_WRITE           0x02        //write data
#define EEPROM_WRDI            0x04        //reset write enable latch
#define EEPROM_WREN            0x06        //set the write enable latch
#define EEPROM_RDSR            0x05        //read status reg
#define EEPROM_WRSR            0x01        //write status reg

//eeprom read/write prototypes
static void eeprom_writeEnable(void);
static void eeprom_writeDisable(void);
static uint8_t eeprom_readStatus(void);
static void eeprom_writeData(uint16_t address, uint8_t data);
static uint8_t eeprom_readData(uint16_t address);




//////////////////////////////////////
//Attributes - 
//For each attribute, we have the following:
//static var - name shown as appears in sys/kernel/myKObject 
//var_show function - read / cat variable
//var_store function - write / echo > variable

//////////////////////////////////////////////
//Attribute - WIP - Write In Progress
//0 - Ready, 1 - Write in Progress
//Read Only
static int wip;

static ssize_t wip_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{

    wip = eeprom_readStatus();
    wip &= 0x01;                    //bit 0

    return sprintf(buf, "%d\n", wip);
}

static struct kobj_attribute wip_attribute = __ATTR_RO(wip);


//////////////////////////////////////////////
//Attribute - WREN - Write Enable
//0 - Write Disable, 1 - Write Enable
//Read/Write
static int wren;

//_show - returns 0 Disable, 1 Enable
static ssize_t wren_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{
    //write enable bit = bit 1
    wren = (eeprom_readStatus() >> 1);
    wren &= 0x01;
  
    return sprintf(buf, "%d\n", wren);
}

//_store - Write 0 Disable, 1 Enable.  If 
//write value out of range, 0 Disable
static ssize_t wren_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    ret = kstrtoint(buf, 10, &temp);
    if (ret < 0)
        return ret;

    //eval wren,
    if ((!temp) || (temp == 1))
    {
        wren = (temp & 0x01);
    }
    else
    {
        wren = 0;           //disable write on error
    }

    if (!wren)
    {
        eeprom_writeDisable();
    }
    else
    {
        eeprom_writeEnable();
    }

    return count;
}

//wren attribute
static struct kobj_attribute wren_attribute =
   __ATTR(wren, 0664, wren_show, wren_store);





//////////////////////////////////////////////
//Attribute - AUTOINC
//Auto increment the destination address on 
//a read or write.  0 - manual, 1 - auto
//
static int autoinc;

//_show - returns 0 Manuual, 1 Auto
static ssize_t autoinc_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{  
    return sprintf(buf, "%d\n", autoinc);
}

//_store - Write 0 Manual, 1 Auto.  If 
//write value out of range, 0 Manual
//
static ssize_t autoinc_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    ret = kstrtoint(buf, 10, &temp);
    if (ret < 0)
        return ret;

    //eval autoinc,
    if ((!temp) || (temp == 1))
    {
        autoinc = (temp & 0x01);
    }
    else
    {
        autoinc = 0;           //set to manual on error
    }

    return count;
}

//autoinc attribute
static struct kobj_attribute autoinc_attribute =
   __ATTR(autoinc, 0664, autoinc_show, autoinc_store);








//////////////////////////////////////////////
//Attribute - ADDR - Current read/write address
//16bit value, entered and displayed as hex value
//requires leading "0x"
//Read/Write
//address auto increments on a read/write eedata
//if autoinc attribute is set to 1
//
static int addr;

//_show - returns current address as hex
static ssize_t addr_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{ 
    return sprintf(buf, "0x%04x\n", addr);
}

//_store - reads in the value as 16bit hex and
//stores in address.
static ssize_t addr_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    //base 16 into address of temp
    ret = kstrtoint(buf, 16, &temp);
    if (ret < 0)
        return ret;

    if (temp < 0)
        addr = 0;
    else if (temp > 0xFFFF)
        addr = 0xFFFF;
    else
        addr = (temp & 0xFFFF);

    return count;
}

//address attribute
static struct kobj_attribute addr_attribute =
   __ATTR(addr, 0664, addr_show, addr_store);







//////////////////////////////////////////////
//Attribute - DATA - Value stored at the 
//current address.  Value is read/written each 
//show/store event.  Value is also stored locally.
//
//Auto increment the eeaddress on a read or
//write if autoinc attribute is set to 1
//
static int data;

//_show - reads and returns value at
//address address. capture bottom 8 bits
//
static ssize_t data_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{ 
    data = (eeprom_readData(addr) & 0xFF);

    //auto increment enabled?
    if (autoinc == 1)
        addr++;

    return sprintf(buf, "0x%02x\n", data);
}

//_store - reads in the value as 8bit hex and
//stores in eedata.  writes data to current
//value of address.
static ssize_t data_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    //base 16 into address of temp
    ret = kstrtoint(buf, 16, &temp);

    if (ret < 0)
        return ret;

    if (temp < 0)
        data = 0;
    else if (temp > 0xFF)
        data = 0xFF;
    else
        data = (temp & 0xFF);

    //write eedata to eeaddress
    eeprom_writeData(addr, data);

    //auto increment enabled?
    if (autoinc == 1)
        addr++;

    return count;
}

//data attribute
static struct kobj_attribute data_attribute =
   __ATTR(data, 0664, data_show, data_store);








////////////////////////////////////////////
//Attribute Group
//An array of attribute pointers
//one entry for each attribute + NULL
//
static struct attribute *attrs[] = {
   &wip_attribute.attr,
   &wren_attribute.attr,
   &autoinc_attribute.attr,
   &addr_attribute.attr,
   &data_attribute.attr,
   NULL,
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
   .attrs = attrs,
};


//////////////////////////////////
//kobject - eeprom_kobj
static struct kobject *eeprom_kobj;



static int __init sysfs_eeprom_init(void)
{
    int retval;

    //////////////////////////////////////////////////
    //make the kobject with name "eeprom" in sys/kernel
    eeprom_kobj = kobject_create_and_add("eeprom", kernel_kobj);
    if (!eeprom_kobj)
      return -ENOMEM;

    //Connect the attributes to the kobject.  This also
    //creates the files associated with the kobject.  
    //also allows file functions in user space, open
    //close, read, write...
    retval = sysfs_create_group(eeprom_kobj, &attr_group);
    if (retval)
      kobject_put(eeprom_kobj);

    return retval;
}


static void __exit sysfs_eeprom_exit(void)
{
   kobject_put(eeprom_kobj);        //destroy the object, remove the files

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
    uint8_t tx = EEPROM_RDSR;
    uint8_t rx = 0xFF;

    //send RDSR followed by read 1 byte
    spi_tx_rx(&tx, 1, &rx, 1);

    return rx;
}


void eeprom_writeData(uint16_t address, uint8_t data)
{
    uint8_t status, lowByte, highByte;
    uint8_t buff[4] = {EEPROM_WRITE, 0x00, 0x00, 0x00};

    //check the status - Write in progress
    //WEL - bit 1 = must be high - write enable latch
    //WIP - bit 0 = must be low - write in progress
    status = eeprom_readStatus();

    //wait for WIP to go low
    while ((status & 0x01) == 1)
    {
        status = eeprom_readStatus();
    }

    //set the write enable latch and poll for WEL bit 1 == 1
    eeprom_writeEnable();
    status = eeprom_readStatus();

    //wait while bit 1 is not high
    while ((status & 0x02) != 2)
    {
        status = eeprom_readStatus();
    }

    //split the high and low bytes
    lowByte = address & 0x00FF;
    highByte = (address >> 8) & 0xFF;

    buff[0] = EEPROM_WRITE;
    buff[1] = highByte;
    buff[2] = lowByte;
    buff[3] = data;

    //send
    spi_tx(buff, 4);

    //check the status - Write in progress
    //WEL - bit 1 = must be high - write enable latch
    //WIP - bit 0 = must be low - write in progress
    status = eeprom_readStatus();

    //wait for WIP to go low
    while ((status & 0x01) == 1)
    {
        status = eeprom_readStatus();
    }

    //now we can leave
}



uint8_t eeprom_readData(uint16_t address)
{
    uint8_t status, lowByte, highByte, result;
    uint8_t buff[] = {EEPROM_READ, 0x00, 0x00};

    //check the status - Write in progress
    //WEL - bit 1 = must be high - write enable latch
    //WIP - bit 0 = must be low - write in progress
    status = eeprom_readStatus();

    //wait for WIP to go low
    while ((status & 0x01) == 1)
    {
        status = eeprom_readStatus();
    }

    //split the high and low bytes
    lowByte = address & 0x00FF;
    highByte = (address >> 8) & 0xFF;

    buff[0] = EEPROM_READ;
    buff[1] = highByte;
    buff[2] = lowByte;

    result = 0x00;

    spi_tx_rx(buff, 3, &result, 1);

    return result;    
}



module_init(sysfs_eeprom_init);
module_exit(sysfs_eeprom_exit);

