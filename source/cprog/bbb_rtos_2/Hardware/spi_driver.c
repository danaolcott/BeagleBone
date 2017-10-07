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
//so just write them here.
//
void spi_init(void)
{
	//this seems to work well
	system("config-pin P9.17 spi");
	system("config-pin P9.18 spi");
	system("config-pin P9.21 spi");
	system("config-pin P9.22 spi");


	//configure as write only....
	//TODO:
	//build up functions for config as write
	//only, write followed by a read, write
	//no cs pin... etc.  Seems like all this
	//takes place in ioctl.
	//
	unsigned int fp;
	uint8_t bits = 8, mode = 0;
	uint32_t speed = 10000;

	fp = open(spiPath, O_RDWR);			//open

	//TODO:
	//seems like we could get rid of all the rd_mode
	//read?  or is it input.

	ioctl(fp, SPI_IOC_WR_MODE, &mode);
	ioctl(fp, SPI_IOC_RD_MODE, &mode);

	ioctl(fp, SPI_IOC_WR_BITS_PER_WORD, &bits);
	ioctl(fp, SPI_IOC_RD_BITS_PER_WORD, &bits);

	ioctl(fp, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	ioctl(fp, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

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

	//write the data
	write(fp, data, length);				//write 1 byte

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
	msg[0].len = length;

	//open the file
	fp = open(spiPath, O_RDWR);			//open

	//send the message
	status = ioctl(fp, SPI_IOC_MESSAGE(1), msg);
	if (status < 0)
	{
		printf("Error - SPI: SPI_IOC_MESSAGE Failed\n");
		return -1;
	}

	return status;
}



int spi_rx(uint8_t* rxBuffer, uint16_t length)
{
	struct spi_ioc_transfer msg[1];
	int status, fp;

	memset(msg, 0, sizeof(msg));

	msg[0].rx_buf = (unsigned long)rxBuffer;
	msg[0].len = length;

	//open the file
	fp = open(spiPath, O_RDWR);			//open

	//send the message
	status = ioctl(fp, SPI_IOC_MESSAGE(1), msg);
	if (status < 0)
	{
		printf("Error - SPI: SPI_IOC_MESSAGE Failed\n");
		return -1;
	}

	return status;
}

//////////////////////////////////////////////////////////
//spi_send_receive_array
//Transmits n bytes from tx bufffer followed by
//receiving n bytes into the rx buffer
//use the spi_ioc_transfer struct
int spi_send_receive_array(int fp, uint8_t* txBuffer, uint8_t* rxBuffer, uint16_t length)
{
	struct spi_ioc_transfer msg[1];
	int status;
	memset(msg, 0, sizeof(msg));

	msg[0].tx_buf = (unsigned long)txBuffer;
	msg[0].rx_buf = (unsigned long)rxBuffer;
	msg[0].len = length;
	msg[0].speed_hz = 10000;
	msg[0].bits_per_word = 8;
	msg[0].delay_usecs = 0;
	msg[0].cs_change = 0;


	//send the message
	status = ioctl(fp, SPI_IOC_MESSAGE(1), msg);
	if (status < 0)
	{
		printf("Error - SPI: SPI_IOC_MESSAGE Failed\n");
		return -1;
	}

	return status;

}
