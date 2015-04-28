/*
 *********************************************************************************************************
 *
 *                        (c) Copyright 2015-2020 RedPine Signals Inc. HYDERABAD, INDIA
 *                                            All rights reserved.
 *
 *               This file is protected by international copyright laws. This file can only be used in
 *               accordance with a license and should not be redistributed in any way without written
 *               permission by Redpine Signals.
 *
 *                                            www.redpinesignals.com
 *
 *********************************************************************************************************
 */
 /*
 *********************************************************************************************************
 *
 *											  main file
 *
 * File           : main.c
 * Version        : V1.00
 * Programmer(s)  :
 * Description    : This example uses Serial Peripheral Interface (SPI) of WyzBee board as master in polling method and OLED screen as slave to print a text message.
 *********************************************************************************************************
 * Note(s)		 :
 *********************************************************************************************************
 */
 
 /*
 * Includes
 */
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1351.h"
#include <SPI_OLED.h>
#include <WyzBee_spi.h>
#include <WyzBee_i2c.h>
#include <WyzBee_gpio.h>
#include <locale.h>



extern Adafruit_SSD1351 tft = Adafruit_SSD1351(); //@  OLED class variable
/*===================================================*/
/**
 * @fn			void print_oled(int8 *text, uint16_t color)
 * @brief		this functions is used to print the data on the oled screen
 * @param 1[in]	int8 *text
 * @param 2[in]	uint16_t color , color
 * @param[out]	none
 * @return		none
 * @description This API should contain the code / function call which will prints the data on the OLED screen.
 */


WyzBeeSpi_Config  config_stc={
		4000000,
		SpiMaster,
		SpiSyncWaitZero,
		SpiEightBits,
		SpiMsbBitFirst,
		SpiClockMarkHigh,
		SpiNoUseIntrmode,
		NULL,                                                                
		NULL
};

WyzBeeI2c_Config config_i2c={
    100000,
    I2cMasterMode,
    I2cNoizeFilterLess100M,
    0x00
};
    


int main()
{    
	WyzBeeSpi_Init(&config_stc); // initialize spi 
    WyzBeeI2c_Init(&config_i2c); // initialize i2c
    Adafruit_SSD1351 myOled;
    myOled.begin(); //@ OLED screen Initialization

    // Prepare screen
    myOled.fillScreen(BLACK); //@ fills the OLED screen with black pixels
    myOled.setTextSize(1); //@ Set the text size on the OLED
    myOled.setTextColor(WHITE, BLACK);

    while(1) {
        if(!WyzBeeGpio_Get(4E))
            continue;
        
        uint8_t MEASURE_HUMID[1] = {0xE5};
        uint8_t MEASURE_TEMP[1] = {0xE3};
        
        char buf[64]={0};
        uint8_t addr = 0x40;
        uint8_t data[4]={0};
        uint16_t len=4;
        int c=0;
        
        WyzBeeI2c_Write(addr, MEASURE_HUMID, 1);
        WyzBeeI2c_Read(addr, data, &len);
        int16_t humidCode = ((int16_t)data[0]<<8)|data[1];
        double humid = (125*(double)humidCode)/65536 - 6;
        c += sprintf(buf+c, "Humidity = %.2lf%%\n", humid);
        buf[c-5] = '.'; // dirty dirty hack


        WyzBeeI2c_Write(addr, MEASURE_TEMP, 1);
        WyzBeeI2c_Read(addr, data, &len);
        int16_t tempCode = ((int16_t)data[0]<<8)|data[1];
        double temp = (175.72*(double)tempCode)/65536 - 46.85;
        c += sprintf(buf+c, "Temperature = %.2lf\n", temp);
        buf[c-4] = '.'; // dirty dirty hack
        
        myOled.setCursor(0, 0); //@ Set the print cursor to the initial location
        myOled.writeString(buf);
    }    
}
/*
 *********************************************************************************************************
 *                                           END
 *********************************************************************************************************
 */
