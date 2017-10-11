////////////////////////////////////////////
//10/9/17
//Dana Olcott
//
//EEPROM Driver File
//Controller file for the Microchip 
//25AA640A/25LC640A Serial EEPROM IC
//SPI interface
//Idle clock low
//Data on the leading edge
//
//This version of the eeprom memory driver
//is intented to be used with BeagleBoneBlack
//and uses the spi_driver files to send and
//receive data.
//Pins: P9-17, P9-18, P9-21, P9-22
//
//
//For EEPROM Writes:
//Read the status bits - can do this even while a write is in progress
//Important to poll the WIP - Write in Progress bit - needs to be low before you can write
//Important to poll the WEl - Write Enable Latch - needs to be high before you can write
//Important to poll the WIP after a transmission.  This was a problem making the
//the SPI work on the BeagleBone
//
//For EEPROM Reads:
//Important to poll the WIP - Write in Progress bit.  Although I don't
//think this has to be low in order to read, I had better luck if you
//do for reads that immediately follow a write.
//
//
#include "main.h"
#include "eeprom_driver.h"
#include "spi_driver.h"


void eeprom_init()
{
    eeprom_writeEnable();
}

void eeprom_writeEnable()
{
    eeprom_writeByte(WREN);
}

void eeprom_writeDisable()
{
    eeprom_writeByte(WRDI);
}

///////////////////////////////////
//Just send the data, no write command
//
void eeprom_writeByte(uint8_t data)
{
	uint8_t tx = data;
    spi_tx(&tx, 1);
}

///////////////////////////////////
//Send 2 bytes:
//WRSR 
//data
void eeprom_writeStatus(uint8_t data)
{
	//send WRSR and the data
	uint8_t buf[] = {WRSR, data};
    spi_tx(buf, 2);
}

void eeprom_writeData(uint16_t address, uint8_t data)
{
    //check the status - Write in progress
    //WEL - bit 1 = must be high - write enable latch
    //WIP - bit 0 = must be low - write in progress
    uint8_t status = eeprom_readStatus();

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
    uint8_t lowByte = address & 0x00FF;
    uint8_t highByte = (address >> 8) & 0xFF;

    //send the following: WRITE, high, low, data
    uint8_t buff[] = {WRITE, highByte, lowByte, data};
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

////////////////////////////////////
//read status bits
//no need to poll the WIP bit because
//you can read this while a write is in progress
uint8_t eeprom_readStatus(void)
{
    uint8_t tx[] = {RDSR};
    uint8_t rx[] = {0xFF};

    //send RDSR followed by read 1 byte
    spi_tx_rx(tx, 1, rx, 1);

    return rx[0];
}

/////////////////////////////////////////////
//Read data from 16bit address
//
uint8_t eeprom_readData(uint16_t address)
{
    //check the status - Write in progress
    //WEL - bit 1 = must be high - write enable latch
    //WIP - bit 0 = must be low - write in progress
    uint8_t status = eeprom_readStatus();

    //wait for WIP to go low
    while ((status & 0x01) == 1)
    {
        status = eeprom_readStatus();
    }

    //split the high and low bytes
    uint8_t lowByte = address & 0x00FF;
    uint8_t highByte = (address >> 8) & 0xFF;

    //send 3 bytes and read 1 byte
    uint8_t buff[] = {READ, highByte, lowByte};
    uint8_t result[] = {0x00};

    spi_tx_rx(buff, 3, result, 1);

    return result[0];
}



