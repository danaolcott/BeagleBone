//////////////////////////////////////////////////
/* 10/6/17
 * Dana Olcott
 * spi_driver.h
 * Header file for spi driver using the /dev/
 * file system.  
 */
#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <stdint.h>


#define SPI_PATH		((char*)"/dev/spidev1.0");
#define SPI_SPEED_HZ	2000000

void spi_init(void);
void spi_writeByte(uint8_t data);
void spi_writeArray(uint8_t* data, uint32_t length);
int spi_tx(uint8_t* txBuffer, uint16_t length);
int spi_rx(uint8_t* rxBuffer, uint16_t length);
int spi_send_receive_array(uint8_t* txBuf, uint8_t* rxBuf, uint16_t len);
int spi_tx_rx(uint8_t* txBuf, uint16_t txLen, uint8_t* rxBuf, uint16_t rxLen);



#endif
