////////////////////////////////////////////////
/* 9/24/17
 * Dana Olcott
 * spi_driver
 *

Configure Pins 9-17, 9-18, 9-21, and 9-22 as
SPI and functions for reading / writing.

Some useful links to get started:

How to configure SPI pins for the loopback test
(only concerned with 17,18,21,22)

https://gist.github.com/pdp7/33a8ad95efcbcc0fadc3f96a70d4b159

Basically, you can set this up in the boot, uEnv.txt file
or write the following on the cli:

config-pin P9.17 spi
config-pin P9.18 spi
config-pin P9.21 spi
config-pin P9.22 spi


To Configure Pins as SPI, from the CLI:

config-pin P9.17 spi - CS
config-pin P9.18 spi - MOSI

config-pin P9.21 spi - MISO
config-pin P9.22 spi - CLK

Might want to consider not configuring the CS pin
via the spi driver and use as normal GPIO.


To configure SPI driver in software, the following
link is very helpful:

http://ulasdikme.com/projects/arduino/BBB_Arduino_SPI.php


//this is a great link about setting up the spi message
//struct:
 * http://brew-j2me.blogspot.com/2010/05/linux-accessing-spi-bus-from-user-space_06.html
 *

 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>				//for cli commands
#include <stdint.h>				//uint8, _16... etc
#include <fcntl.h>				//O_RDWR
#include <unistd.h>				//open, close, read, write... etc
#include <sys/ioctl.h>			//spi mode writes
#include <linux/spi/spidev.h>

#include "spi_driver.h"

char* spiPath = "/dev/spidev1.0";


///////////////////////////////////////////
//spi init
//configure the pins here.   I tried doing this in
//the /boot/uEnv.txt file, but nothing seems to work
//always need to write in the following commands over the cli
//
//
void spi_init(void)
{
	//config via terminal commands
	system("config-pin P9.17 spi");
	system("config-pin P9.18 spi");
	system("config-pin P9.21 spi");
	system("config-pin P9.22 spi");

	unsigned int fp;
	uint8_t bits = 8, mode = 0;
	uint32_t speed = SPI_SPEED_HZ;

	fp = open(spiPath, O_RDWR);			//open

	ioctl(fp, SPI_IOC_WR_MODE, &mode);
	ioctl(fp, SPI_IOC_WR_BITS_PER_WORD, &bits);
	ioctl(fp, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

	fflush(stdout);
	close(fp);
}


//////////////////////////////////////
//SPI init configures the spi
void spi_writeByte(uint8_t data)
{
	unsigned int fp;
	uint8_t c = data;

	fp = open(spiPath, O_RDWR);			//open
	write(fp, &c, 1);				//write 1 byte
	fflush(stdout);
	close(fp);
}


void spi_writeArray(uint8_t* data, uint32_t length)
{
	unsigned int fp;

	fp = open(spiPath, O_RDWR);			//open
	write(fp, data, length);
	fflush(stdout);
	close(fp);
}


/////////////////////////////////////////
//SPI write array alternate
//This approach uses the spi_ioc_transfer
//message struct


int spi_tx(uint8_t* txBuffer, uint16_t length)
{
	struct spi_ioc_transfer msg[1];
	int status, fp;

	memset(msg, 0, sizeof(msg));

	msg[0].tx_buf = (unsigned long)txBuffer;
	msg[0].rx_buf = (unsigned long)NULL;
	msg[0].len = length;
	msg[0].speed_hz = SPI_SPEED_HZ;
	msg[0].bits_per_word = 8;
	msg[0].delay_usecs = 20;
	msg[0].cs_change = 0;			//disable cs between transfers = 0

	//open the file
	fp = open(spiPath, O_RDWR);			//open

	//send the message
	status = ioctl(fp, SPI_IOC_MESSAGE(1), msg);
	if (status < 0)
	{
		printf("Error - SPI: SPI_IOC_MESSAGE Failed\n");
		return -1;
	}

	close(fp);

	return status;
}



int spi_rx(uint8_t* rxBuffer, uint16_t length)
{
	struct spi_ioc_transfer msg[1];
	int status, fp;

	memset(msg, 0, sizeof(msg));

	msg[0].tx_buf = (unsigned long)NULL;
	msg[0].rx_buf = (unsigned long)rxBuffer;
	msg[0].len = length;
	msg[0].speed_hz = SPI_SPEED_HZ;
	msg[0].bits_per_word = 8;
	msg[0].delay_usecs = 20;
	msg[0].cs_change = 0;			//disable cs between transfers = 0

	//open the file
	fp = open(spiPath, O_RDWR);			//open

	//send the message
	status = ioctl(fp, SPI_IOC_MESSAGE(1), msg);
	if (status < 0)
	{
		printf("Error - SPI: SPI_IOC_MESSAGE Failed\n");
		return -1;
	}

	close(fp);

	return status;
}

//////////////////////////////////////////////////////////
//spi_send_receive_array
//Transmits n bytes from tx bufffer followed by
//receiving n bytes into the rx buffer
//use the spi_ioc_transfer struct.
//CS pin statys low between tx and rx
//tx and rx num bytes need to be the same

//len = total number of bytes written + read back.
int spi_send_receive_array(uint8_t* txBuf, uint8_t* rxBuf, uint16_t len)
{
	struct spi_ioc_transfer msg[1];
	int status, fp;
	memset(msg, 0, sizeof(msg));

	//open the file
	fp = open(spiPath, O_RDWR);			//open

	msg[0].tx_buf = (unsigned long)txBuf;
	msg[0].rx_buf = (unsigned long)rxBuf;
	msg[0].len = len;
	msg[0].speed_hz = SPI_SPEED_HZ;
	msg[0].bits_per_word = 8;
	msg[0].delay_usecs = 20;
	msg[0].cs_change = 0;

	//send the message
	status = ioctl(fp, SPI_IOC_MESSAGE(1), msg);
	if (status < 0)
	{
		printf("Error - SPI: SPI_IOC_MESSAGE Failed\n");
		return -1;
	}

	close(fp);

	return status;

}

//////////////////////////////////////////////////////////
//SPI tx/rx using variable length buffers
//CS pin stays low between transmissions
//uses two spi_ior_transfer msg structs, one
//for tx, one for rx.
int spi_tx_rx(uint8_t* txBuf, uint16_t txLen, uint8_t* rxBuf, uint16_t rxLen)
{
	struct spi_ioc_transfer msg[2];
	int status, fp;

	memset(&msg[0], 0, sizeof(msg[0]));
	memset(&msg[1], 0, sizeof(msg[1]));

	//open the file
	fp = open(spiPath, O_RDWR);			//open

	//init the tx struct
	msg[0].tx_buf = (unsigned long)txBuf;
	msg[0].rx_buf = (unsigned long)NULL;
	msg[0].len = txLen;
	msg[0].speed_hz = SPI_SPEED_HZ;
	msg[0].bits_per_word = 8;
	msg[0].delay_usecs = 50;
	msg[0].cs_change = 0;			//disable cs between transfers = 0

	//init the rx struct
	msg[1].tx_buf = (unsigned long)NULL;
	msg[1].rx_buf = (unsigned long)rxBuf;
	msg[1].len = rxLen;
	msg[1].speed_hz = SPI_SPEED_HZ;
	msg[1].bits_per_word = 8;
	msg[1].delay_usecs = 50;
	msg[1].cs_change = 0;			//disable cs between transfers = 0


	//initiate 2 transfers , msg[0] and msg[1]
	status = ioctl(fp, SPI_IOC_MESSAGE(2), msg);
	if (status < 0)
	{
		printf("Error - SPI: SPI_IOC_MESSAGE Failed\n");
		return -1;
	}

	close(fp);

	return status;
}
