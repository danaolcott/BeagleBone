////////////////////////////////////////////
/*
10/31/17
Dana Olcott
EEPROM Driver File
Controller file for the Microchip
25AA640A/25LC640A Serial EEPROM IC

This driver file interacts with sysfs
eeprom module via the spi_interface module.

Files Available for read/write:
/sys/kernel/eeprom/addr			address
/sys/kernel/eeprom/autoinc		auto increment address
/sys/kernel/eeprom/data			read/write data at the address
/sys/kernel/eeprom/wip			write in progress - don't really need this
/sys/kernel/eeprom/wren			write enable - don't really need this

Note: add read values are readback in hex

*/

#ifndef EEPROM_DRIVER__H
#define EEPROM_DRIVER__H

#include <stdio.h>
#include <stdint.h>

#define EEPROM_PATH_ADDR	((char*)"/sys/kernel/eeprom/addr")
#define EEPROM_PATH_AUTOINC	((char*)"/sys/kernel/eeprom/autoinc")
#define EEPROM_PATH_DATA	((char*)"/sys/kernel/eeprom/data")

void eeprom_init(void);
void eeprom_autoinc_enable(void);
void eeprom_autoinc_disable(void);
void eeprom_writeData(uint16_t address, uint8_t data);
uint8_t eeprom_readData(uint16_t address);

#endif

