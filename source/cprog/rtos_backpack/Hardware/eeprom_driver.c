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


#include "eeprom_driver.h"
#include <stdlib.h>		//system call to run spi_config
#include <fcntl.h>		//O_RDWR
#include <unistd.h>		//open, close, read, write... etc
#include <string.h>		//sprintf

//////////////////////////////////
//eeprom_init
//spi_interface and sysfs_eeprom modules
//need to be installed before running this file
//Run the spi config program again just to
//make sure
void eeprom_init(void)
{
	//run the spi setup program to ensure pins are
	//configured properly.
	//
	printf("Running spi_config again...\n");
    system("/home/debian/lkm/spi_interface/spi_config 0 8 1000000");
}

//////////////////////////////////////
//enable auto increment the address on each
//data read/write
void eeprom_autoinc_enable(void)
{
	int fp, n;
	char buff[10];
	memset(buff, 0x00, 10);

	//printf 1 into autoinc file
	n = sprintf(buff, "1");

	//set the address using write to addr
	fp = open(EEPROM_PATH_AUTOINC, O_RDWR);	//open for read/write
	write(fp, buff, n);
	close(fp);

}

//////////////////////////////////////
//disable auto increment the address on
//each data read/write
void eeprom_autoinc_disable(void)
{
	int fp, n;
	char buff[10];
	memset(buff, 0x00, 10);

	//printf 1 into autoinc file
	n = sprintf(buff, "0");

	//set the address using write to addr
	fp = open(EEPROM_PATH_AUTOINC, O_RDWR);	//open for read/write
	write(fp, buff, n);
	close(fp);

}

//////////////////////////////////////////////////
//eeprom_writeData
//write 8 bit data to 16 bit address.
//makes 2 writes to sys/kernel/eeprom
//write to addr to set the write address
//write to data file to write data to eeprom
//
void eeprom_writeData(uint16_t address, uint8_t data)
{
	int fp, n;
	char buff[10];

	//set the write address by writing  to lcd/addr
	memset(buff, 0x00, 10);
	n = sprintf(buff, "0x%04x", address);
	fp = open(EEPROM_PATH_ADDR, O_RDWR);	//open for read/write
	write(fp, buff, n);
	close(fp);

	//write the data by writing to lcd/data
	memset(buff, 0x00, 10);
	n = sprintf(buff, "0x%02x", data);
	fp = open(EEPROM_PATH_DATA, O_RDWR);	//open for read/write
	write(fp, buff, n);
	close(fp);

}

/////////////////////////////////////////////
//eeprom_readData
//reads the 8bit value from address
//makes one write and one read:
//write to addr to set the address
//read the contents of data at the given address
//
uint8_t eeprom_readData(uint16_t address)
{
	int fp, bytesRead, n;
	char buff[10];
	memset(buff, 0x00, 10);
	int value = 0x00;

	//printf the address into buffer as hex
	n = sprintf(buff, "0x%04x", address);

	//set the address using write to addr
	fp = open(EEPROM_PATH_ADDR, O_RDWR);	//open for read/write
	write(fp, buff, n);
	close(fp);

	//open the file again and read the contents
	//of the addr file
	memset(buff, 0x00, 10);
	fp = open(EEPROM_PATH_DATA, O_RDWR);	//open for read/write
	bytesRead = read(fp, buff, 10);		//read value, max buffer 10
	close(fp);							//close

	if (bytesRead > 0)
		value = (int)strtol(buff, NULL, 16);

	return (uint8_t)value;
}
