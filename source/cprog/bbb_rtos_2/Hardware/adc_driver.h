//////////////////////////////////////////////////
/* 10/5/17
 * Dana Olcott
 * adc_driver.h
 * Header file for adc driver using the /dev/
 * file system.  
 */
#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>


//device file paths for each adc channel
#define ADC_CH0_PATH		((char*)"/sys/bus/iio/devices/iio:device0/in_voltage0_raw");
#define ADC_CH1_PATH		((char*)"/sys/bus/iio/devices/iio:device0/in_voltage1_raw");
#define ADC_CH2_PATH		((char*)"/sys/bus/iio/devices/iio:device0/in_voltage2_raw");
#define ADC_CH3_PATH		((char*)"/sys/bus/iio/devices/iio:device0/in_voltage3_raw");
#define ADC_CH4_PATH		((char*)"/sys/bus/iio/devices/iio:device0/in_voltage4_raw");
#define ADC_CH5_PATH		((char*)"/sys/bus/iio/devices/iio:device0/in_voltage5_raw");
#define ADC_CH6_PATH		((char*)"/sys/bus/iio/devices/iio:device0/in_voltage6_raw");

typedef enum
{
	ADC_CH0,
	ADC_CH1,
	ADC_CH2,
	ADC_CH3,
	ADC_CH4,
	ADC_CH5,
	ADC_CH6,
}AdcChannel_t;

void adc_init(void);
uint16_t adc_readChannel_mV(AdcChannel_t ch);
uint16_t adc_readChannel_raw(AdcChannel_t ch);


#endif
