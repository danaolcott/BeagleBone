////////////////////////////////////////////////
/* 9/24/17
 * Dana Olcott
 * adc_driver
 *
 * Read adc values from 1.8v adc pins on the 
 * BeagleBoneBlack
 */


#include "adc_driver.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		//O_RDWR
#include <unistd.h>		//open, close, read, write... etc
#include <string.h>


/////////////////////////////
//not sure what goes here...
void adc_init(void)
{
	//TBD
}


uint16_t adc_readChannel_raw(AdcChannel_t ch)
{
	char* adcPath = ADC_CH0_PATH;
	int fp;
	char buff[16];
	uint16_t value, bytesRead;	
	memset(buff, 0x00, 16);

	switch(ch)
	{
		case ADC_CH0:
			adcPath = ADC_CH0_PATH;
			break;
		case ADC_CH1:
			adcPath = ADC_CH1_PATH;
			break;
		case ADC_CH2:
			adcPath = ADC_CH2_PATH;
			break;
		case ADC_CH3:
			adcPath = ADC_CH3_PATH;
			break;
		case ADC_CH4:
			adcPath = ADC_CH4_PATH;
			break;
		case ADC_CH5:
			adcPath = ADC_CH5_PATH;
			break;
		case ADC_CH6:
			adcPath = ADC_CH6_PATH;
			break;
	}

	fp = open(adcPath, O_RDWR);			//open
	bytesRead = read(fp, buff, 16);		//read into buff, max 10 bytes
	close(fp);							//close

	if (bytesRead > 0)
	{
		value = atoi(buff);

		//value should be 0 to 4095
		if ((value < 0) || (value > 4095))
			value = 0;
	}
	else
		value = 0;

	return value;	
}


/////////////////////////////////////////
//Read channel x and return value in mv
//use the read raw data function
uint16_t adc_readChannel_mV(AdcChannel_t ch)
{
	uint16_t value, rawData;
	rawData = adc_readChannel_raw(ch);
	value = rawData * 1800 / 4095;

	return value;	
}

