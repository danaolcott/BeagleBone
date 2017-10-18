////////////////////////////////////////////////////
/*
LCD Controller Module for the EA 16x3
Character Display PN#: EADOGM163LA
Display is configured for a 4bit interface

This module uses the following functions
exported from the following module:
/home/debian/lkm/lcd_interface:

lcd_init()
lcd_reset()
lcd_clear()
lcd_cursorOn()
lcd_cursorOff()
lcd_writeString(buf, line, offset)
lcd_writeStringBytes(buf, length, line, offset)
lcd_setContrast()

Sysfs Attributes:

line0 - Text String on Line 0 - Max Chars 16
line1 - Text String on Line 1 - Max Chars 16
line2 - Text String on Line 2 - Max Chars 16
cursor - 0 off, 1 on
contrast - 0 to 15


*/
///////////////////////////////////////////////////

#include <linux/kobject.h>      //kobject
#include <linux/string.h>       //string to int
#include <linux/sysfs.h>        //sysfs class
#include <linux/module.h>
#include <linux/init.h>         //__init/__exit
#include <linux/stat.h>         //??
#include <asm/uaccess.h>        //copy to/from user


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dana Olcott");
MODULE_DESCRIPTION("LCD Controller Module");
MODULE_VERSION("0.1");

//////////////////////////////////////////////////
//Exported functions to be declared as extern
//here (defined in lcd_interface lkm module):
//lcd_interface.ko needs to be installed prior
//to loading this module to work properly

#define LCD_LINE_WIDTH          ((int)16)
#define LCD_CONTRAST_DEFAULT    ((int)11)


extern void lcd_init(void);
extern void lcd_reset(void);
extern void lcd_clear(void);
extern void lcd_cursorOn(void);
extern void lcd_cursorOff(void);
extern void lcd_writeString(char* buf, uint8_t line, uint8_t offset);
extern void lcd_writeStringBytes(char* buf, uint8_t length, uint8_t line, uint8_t offset);
extern void lcd_setContrast(uint8_t contrast);


//////////////////////////////////////
//Attributes - 
//For each attribute, we have the following:
//static var - name shown as appears in sys/kernel/myKObject 
//var_show function - read / cat variable
//var_store function - write / echo > variable

static char line0[LCD_LINE_WIDTH];
static char line1[LCD_LINE_WIDTH];
static char line2[LCD_LINE_WIDTH];
static int cursor;                  //0 off, 1 on
static int contrast;                //0 to 15

///////////////////////////////////////////
//Attribute line0 - show
//Return text string on line0
//
static ssize_t line0_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{ 
    return sprintf(buf, ">%s<\n", line0);
}

///////////////////////////////////////////
//Attribute line0 - store 
//Write up to 16 chars on line0 
//
static ssize_t line0_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int i, len;
    memset(line0, 0x00, LCD_LINE_WIDTH);
    strncpy(line0, buf, LCD_LINE_WIDTH);

    //strip line endings, replace all _ with space
    for (i = 0 ; i < LCD_LINE_WIDTH ; i++)
    {
        if ((line0[i] == '\n') || (line0[i] == '\r'))
            line0[i] = 0x00;
        else if (line0[i] == '_')
            line0[i] = ' ';
    }

    len = strnlen(line0, LCD_LINE_WIDTH);    
    printk(KERN_EMERG "ECHO: Copied %d chars into line0 (str len: %d, count: %d): >%s<\n", 
        len, len, count, line0);

    //write len chars from line0 into the lcd
    lcd_writeStringBytes(line0, len, 0, 0);

    return count;
}



///////////////////////////////////////////
//Attribute line1 - show
//Return text string on line1
//
static ssize_t line1_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{ 
    return sprintf(buf, ">%s<\n", line1);
}

///////////////////////////////////////////
//Attribute line1 - store 
//Write up to 16 chars on line1 
//
static ssize_t line1_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int i, len;
    memset(line1, 0x00, LCD_LINE_WIDTH);
    strncpy(line1, buf, LCD_LINE_WIDTH);

    //strip line endings, replace all _ with space
    for (i = 0 ; i < LCD_LINE_WIDTH ; i++)
    {
        if ((line1[i] == '\n') || (line1[i] == '\r'))
            line1[i] = 0x00;
        else if (line1[i] == '_')
            line1[i] = ' ';
    }

    len = strnlen(line1, LCD_LINE_WIDTH);    
    printk(KERN_EMERG "ECHO: Copied %d chars into line1 (str len: %d, count: %d): >%s<\n", 
        len, len, count, line1);

    //write len chars from line1 into the lcd
    lcd_writeStringBytes(line1, len, 0, 0);

    return count;
}




///////////////////////////////////////////
//Attribute line2 - show
//Return text string on line2
//
static ssize_t line2_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{ 
    return sprintf(buf, ">%s<\n", line2);
}

///////////////////////////////////////////
//Attribute line2 - store 
//Write up to 16 chars on line2 
//
static ssize_t line2_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int i, len;
    memset(line2, 0x00, LCD_LINE_WIDTH);
    strncpy(line2, buf, LCD_LINE_WIDTH);

    //strip line endings, replace all _ with space
    for (i = 0 ; i < LCD_LINE_WIDTH ; i++)
    {
        if ((line2[i] == '\n') || (line2[i] == '\r'))
            line2[i] = 0x00;
        else if (line2[i] == '_')
            line2[i] = ' ';
    }

    len = strnlen(line2, LCD_LINE_WIDTH);    
    printk(KERN_EMERG "ECHO: Copied %d chars into line2 (str len: %d, count: %d): >%s<\n", 
        len, len, count, line2);

    //write len chars from line2 into the lcd
    lcd_writeStringBytes(line2, len, 0, 0);

    return count;
}











//////////////////////////////////////////////
//Attribute - cursor
//0 - off, 1 - on
//
static ssize_t cursor_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{  
    return sprintf(buf, "%d\n", cursor);
}


//store
static ssize_t cursor_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    ret = kstrtoint(buf, 10, &temp);
    if (ret < 0)
        return ret;

    //eval cursor 0 or 1
    if ((!temp) || (temp == 1))
    {
        cursor = (temp & 0x01);
    }
    else
    {
        cursor = 0;         //off on error
    }

    if (!cursor)
        lcd_cursorOff();
    else
        lcd_cursorOn();

    return count;
}






//////////////////////////////////////////////
//Attribute - contrast
//0 to 15 or default
//
static ssize_t contrast_show(struct kobject *kobj, 
    struct kobj_attribute *attr, char *buf)
{  
    return sprintf(buf, "%d\n", contrast);
}


//store
static ssize_t contrast_store(struct kobject *kobj, struct kobj_attribute *attr,
          const char *buf, size_t count)
{
    int ret, temp;

    ret = kstrtoint(buf, 10, &temp);
    if (ret < 0)
        return ret;

    //eval contrast value 0 to 15
    if ((temp >=0) && (temp <=15))
    {
        contrast = temp;
    }
    else
    {
        contrast = LCD_CONTRAST_DEFAULT;
    }

    lcd_setContrast(contrast);

    return count;
}








/////////////////////////////////////////////
//Attribute Definitions
//
static struct kobj_attribute line0_attribute =
   __ATTR(line0, 0664, line0_show, line0_store);

static struct kobj_attribute line1_attribute =
   __ATTR(line1, 0664, line1_show, line1_store);

static struct kobj_attribute line2_attribute =
   __ATTR(line2, 0664, line2_show, line2_store);

static struct kobj_attribute cursor_attribute =
   __ATTR(cursor, 0664, cursor_show, cursor_store);

static struct kobj_attribute contrast_attribute =
   __ATTR(contrast, 0664, contrast_show, contrast_store);




////////////////////////////////////////////
//Attribute Group
//An array of attribute pointers
//one entry for each attribute + NULL
//
static struct attribute *attrs[] = {
   &line0_attribute.attr,
   &line1_attribute.attr,
   &line2_attribute.attr,
   &cursor_attribute.attr,
   &contrast_attribute.attr,
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
static struct kobject *lcd_kobj;



static int __init sysfs_lcd_init(void)
{
    int retval;
    int a, b, c;

    //assign initial values and write
    a = sprintf(line0, "Hello-Line0");
    b = sprintf(line1, "Hello-Line1");
    c = sprintf(line2, "Hello-Line2");

    cursor = 0;
    contrast = LCD_CONTRAST_DEFAULT;

    lcd_clear();
    lcd_writeStringBytes(line0, a, 0, 0);
    lcd_writeStringBytes(line1, b, 1, 0);
    lcd_writeStringBytes(line2, c, 2, 0);
    lcd_cursorOff();
    lcd_setContrast(contrast);


    //////////////////////////////////////////////////
    //make the kobject with name "eeprom" in sys/kernel
    lcd_kobj = kobject_create_and_add("lcd", kernel_kobj);
    if (!lcd_kobj)
      return -ENOMEM;

    //Connect the attributes to the kobject.  This also
    //creates the files associated with the kobject.  
    //also allows file functions in user space, open
    //close, read, write...
    retval = sysfs_create_group(lcd_kobj, &attr_group);
    if (retval)
      kobject_put(lcd_kobj);

    return retval;
}


////////////////////////////////////////////
//Goodby message and restore to default values
static void __exit sysfs_lcd_exit(void)
{
    int a, b, c;

    //assign initial values and write
    a = sprintf(line0, "Goodby-Line0");
    b = sprintf(line1, "Goodby-Line1");
    c = sprintf(line2, "Goodby-Line2");

    cursor = 0;
    contrast = LCD_CONTRAST_DEFAULT;

    lcd_clear();
    lcd_writeStringBytes(line0, a, 0, 0);
    lcd_writeStringBytes(line1, b, 1, 0);
    lcd_writeStringBytes(line2, c, 2, 0);
    lcd_cursorOff();
    lcd_setContrast(contrast);

    kobject_put(lcd_kobj);        //destroy the object, remove the files
}







module_init(sysfs_lcd_init);
module_exit(sysfs_lcd_exit);

