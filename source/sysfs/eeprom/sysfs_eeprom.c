////////////////////////////////////////////////////
/*
EEPROM Controller Module for the Microchip
25AA640A/25LC640A Serial EEPROM IC

Attributes
EEWIP - Write In Progress (0 or 1)
    - Value from IC and Stored

EEWREN - Write Enable (0 or 1)
    - Value to/from IC and Stored

EEADDRESS - Current Read/Write Address
    - Value stored locally

EEDATA - Value Stored at the Current Address
    - Value to/from IC and Stored

EEAUTO - Auto increment (0 or 1)
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
#include <linux/stat.h>         //read/write macro defs


MODULE_LICENSE("GPL v2");
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
//Attribute - EEWIP - Write In Progress
//0 - Ready, 1 - Write in Progress
//Read Only
static int eewip;

static ssize_t eewip_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{

    eewip = eeprom_readStatus();
    eewip &= 0x01;                    //bit 0

    return sprintf(buf, "%d\n", eewip);
}

static struct kobj_attribute eewip_attribute = __ATTR_RO(eewip);


//////////////////////////////////////////////
//Attribute - EEWREN - Write Enable
//0 - Write Disable, 1 - Write Enable
//Read/Write
static int eewren;

//_show - returns 0 Disable, 1 Enable
static ssize_t eewren_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{
    //write enable bit = bit 1
    eewren = (eeprom_readStatus() >> 1);
    eewren &= 0x01;
  
    return sprintf(buf, "%d\n", eewren);
}

//_store - Write 0 Disable, 1 Enable.  If 
//write value out of range, 0 Disable
static ssize_t eewren_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    ret = kstrtoint(buf, 10, &temp);
    if (ret < 0)
        return ret;

    //eval wren,
    if ((!temp) || (temp == 1))
    {
        eewren = (temp & 0x01);
    }
    else
    {
        eewren = 0;           //disable write on error
    }

    if (!eewren)
    {
        eeprom_writeDisable();
    }
    else
    {
        eeprom_writeEnable();
    }

    return count;
}

//eewren attribute
static struct kobj_attribute eewren_attribute =
   __ATTR(eewren, 0664, eewren_show, eewren_store);





//////////////////////////////////////////////
//Attribute - EEAUTO
//Auto increment the destination address on 
//a read or write.  0 - manual, 1 - auto
//
static int eeauto;

//_show - returns 0 Manuual, 1 Auto
static ssize_t eeauto_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{  
    return sprintf(buf, "%d\n", eeauto);
}

//_store - Write 0 Manual, 1 Auto.  If 
//write value out of range, 0 Manual
//
static ssize_t eeauto_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    ret = kstrtoint(buf, 10, &temp);
    if (ret < 0)
        return ret;

    //eval eeauto,
    if ((!temp) || (temp == 1))
    {
        eeauto = (temp & 0x01);
    }
    else
    {
        eeauto = 0;           //set to manual on error
    }

    return count;
}

//eeauto attribute
static struct kobj_attribute eeauto_attribute =
   __ATTR(eeauto, 0664, eeauto_show, eeauto_store);








//////////////////////////////////////////////
//Attribute - EEADDRESS - Current read/write address
//16bit value, entered and displayed as hex value
//requires leading "0x"
//Read/Write
//eeaddress auto increments on a read/write eedata
//if eeauto attribute is set to 1
//
static int eeaddress;

//_show - returns current address as hex
static ssize_t eeaddress_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{ 
    return sprintf(buf, "0x%04x\n", eeaddress);
}

//_store - reads in the value as 16bit hex and
//stores in eeaddress.
static ssize_t eeaddress_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    //base 16 into address of temp
    ret = kstrtoint(buf, 16, &temp);
    if (ret < 0)
        return ret;

    if (temp < 0)
        eeaddress = 0;
    else if (temp > 0xFFFF)
        eeaddress = 0xFFFF;
    else
        eeaddress = (temp & 0xFFFF);

    return count;
}

//eeaddress attribute
static struct kobj_attribute eeaddress_attribute =
   __ATTR(eeaddress, 0664, eeaddress_show, eeaddress_store);







//////////////////////////////////////////////
//Attribute - EEDATA - Value stored at the 
//current address.  Value is read/written each 
//show/store event.  Value is also stored locally.
//
//Auto increment the eeaddress on a read or
//write if eeauto attribute is set to 1
//
static int eedata;

//_show - reads and returns value at
//address eeaddress. capture bottom 8 bits
//
static ssize_t eedata_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{ 
    eedata = (eeprom_readData(eeaddress) & 0xFF);

    //auto increment enabled?
    if (eeauto == 1)
        eeaddress++;

    return sprintf(buf, "0x%02x\n", eedata);
}

//_store - reads in the value as 8bit hex and
//stores in eedata.  writes eedata to current
//value of eeaddress.
static ssize_t eedata_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    //base 16 into address of temp
    ret = kstrtoint(buf, 16, &temp);

    if (ret < 0)
        return ret;

    if (temp < 0)
        eedata = 0;
    else if (temp > 0xFF)
        eedata = 0xFF;
    else
        eedata = (temp & 0xFF);

    //write eedata to eeaddress
    eeprom_writeData(eeaddress, eedata);

    //auto increment enabled?
    if (eeauto == 1)
        eeaddress++;

    return count;
}

//eedata attribute
static struct kobj_attribute eedata_attribute =
   __ATTR(eedata, 0664, eedata_show, eedata_store);








////////////////////////////////////////////
//Attribute Group
//An array of attribute pointers
//one entry for each attribute + NULL
//
static struct attribute *attrs[] = {
   &eewip_attribute.attr,
   &eewren_attribute.attr,
   &eeauto_attribute.attr,
   &eeaddress_attribute.attr,
   &eedata_attribute.attr,
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

