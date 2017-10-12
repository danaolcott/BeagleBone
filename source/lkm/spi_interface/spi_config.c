/*
Dana Olcott
10/11/17

SPI Config  a simple c program to configure
spidev1.0 pins as spi and initialize
spi speed, mode, etc.  

This is intended to work on the BeagleBoneBlack
and use SPIDEV1.0.  The following pins are used:

config-pin P9.17 spi - CS         - Configure as Normal IO
config-pin P9.18 spi - MOSI

config-pin P9.21 spi - MISO
config-pin P9.22 spi - CLK

To Configure Pins as SPI, from the CLI:

config-pin P9.17 output - configure as normal io
config-pin P9.18 spi
config-pin P9.21 spi
config-pin P9.22 spi

Have to set the CS pin high/low in the module
as user space does not have access to linux/gpio.h


*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>             //uint8, _16... etc
#include <stdlib.h>             //"system" function
#include <fcntl.h>              //O_RDWR
#include <unistd.h>             //open, close, read, write... etc
#include <sys/ioctl.h>          //spi mode writes
#include <linux/spi/spidev.h>

#define SPI_DEFAULT_MODE          ((uint8_t)0)
#define SPI_DEFAULT_BITS          ((uint8_t)8)
#define SPI_DEFAULT_SPEED_HZ      ((uint32_t)1000000)


char* spiPath = "/dev/spidev1.0";

//////////////////////////////////
//args for main:
//requires 3 arguments:
//mode 0, 1, 2, 3
//bits
//speed = hz
//
int main( int argc, char *argv[] )
{
    unsigned int fp, value;
    uint8_t bits = 8, mode = 0;
    uint32_t speed = 1000000;

    if (argc != 4)
    {
      printf("Invalid Args: mode, bits, speed, ex: 0, 8, 1000000\n");
      return 0;
    }

    //arg1 - mode - default is 0, out of range, set to 0
    mode = ((uint8_t)atoi(argv[1]));
    bits = ((uint8_t)atoi(argv[2]));
    speed = ((uint32_t)atoi(argv[3]));

    if (mode > 3)
      mode = SPI_DEFAULT_MODE;
    if ((bits < 8) || (bits > 32))
      bits = SPI_DEFAULT_BITS;
    if ((speed < 1000000) || (speed > 10000000))
      speed = SPI_DEFAULT_SPEED_HZ;

    printf("Configure SPI Pins: Mode: %d, Bits: %d, Speed(hz): %d\n",
        mode, bits, speed);
    
    ////////////////////////////////////////
    //Configure pins as SPI - MOSI, MISO, CLK
    //Configure CS pin as normal IO, OUTPUT
    //CS Pin is P9.17, which is GPIO5

    //Configure SPI pins
    //config via terminal commands
    system("config-pin P9.17 output");
    system("config-pin P9.18 spi");
    system("config-pin P9.21 spi");
    system("config-pin P9.22 spi");

    //configure the mode, bits, speed
    fp = open(spiPath, O_RDWR);     //open

    ioctl(fp, SPI_IOC_WR_MODE, &mode);
    ioctl(fp, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(fp, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    fflush(stdout);
    close(fp);

    return 0;
}

