 
 /*
 * Includes
 */
#include "main.h"

#include <stdio.h>
#include <string.h>

#include <Adafruit_GFX.h>
#include "Adafruit_SSD1351.h"
#include <SPI_OLED.h>
#include <WyzBee_spi.h>
#include <WyzBee_i2c.h>
#include <WyzBee_gpio.h>
#include <WyzBee.h>

Adafruit_SSD1351* oled = NULL;

int init()
{
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
    
    WyzBeeSpi_Init(&config_stc); // initialize spi
    WyzBeeI2c_Init(&config_i2c); // initialize i2c
    delete oled;
    oled = new Adafruit_SSD1351();
    oled->begin(); //@ OLED screen Initialization
}

void delay2(uint32_t _delay)  // delay
{
	uint32_t x;
	for(x=0;x<100*_delay;x++);
}

int main()
{
    init();
    
    while(1) {
        setColor(blue);
    }
}

int IRTest()
{
    char buf[336];
    char* head = buf;
    
    // Prepare screen
    oled->fillScreen(BLACK); //@ fills the OLED screen with black pixels
    oled->setTextSize(1); //@ Set the text size on the OLED
    oled->setTextColor(WHITE, BLACK);

    while(1) {
        uint8_t ir = WyzBeeGpio_Get(10);
        head = printLineBreak(head);
        //head = printRawData(head, 0, &ir, 1);
        
        oled->setCursor(0, 0);
        oled->writeString(buf);
    }
}

char* printLineBreak(char* buf)
{
    int c=0;
    c += sprintf(buf+c,"---------------------\n");
    return buf+c;
}

char* printRawData(char* buf, uint8_t addr, uint8_t* data, uint16_t len, bool incrementAddress)
{
    int c=0;
    //c += sprintf(buf+c,"\n");
    for(int i = 0; i < len; i=i+4) {
        c += sprintf(buf+c,"0x%02X:",addr + (incrementAddress ? i : 0));
        for(int b = 0; b < 4; b++)
            c+= sprintf(buf+c, " %02X", data[i+b]);
        c += sprintf(buf+c,"\n");
    }
    return buf+c;
}


void setColor(color_t color)
{    
    WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_HIGH);
    WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_HIGH);
    WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_HIGH);
    switch(color) {
        case red:
            /*Toggling Red LED*/
            WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_LOW);
            break;

        case green:
            /*Toggling Green LED*/
            WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_LOW);
            break;
        
        case blue:
            /*Toggling Blue LED*/
            WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
            break;
        
        // SHOULD NEVER HAPPEN
        default:
            WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_LOW);
            WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_LOW);
            WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
            delay2(2000);
    }
}

/*
int tempAndHumidTest()
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
}*/

/*
int accelTest()
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

        char buf[128]={0};
        uint8_t addr = 0x68;
        uint8_t reg = 0x3B;
        uint8_t data[16]={0};
        uint16_t len=6;
        
        WyzBeeI2c_Write(addr, &reg, 1);
        WyzBeeI2c_Read(addr, data, &len);
        
        int16_t accXCode, accYCode, accZCode;
        float accX, accY, accZ;

        accXCode = ((int16_t)data[0]<<8)|data[1];
        accYCode = ((int16_t)data[2]<<8)|data[3];
        accZCode = ((int16_t)data[4]<<8)|data[5];

        accX = (9.8/16384) * accXCode;
        accY = (9.8/16384) * accYCode;
        accZ = (9.8/16384) * accZCode;
        
        int c = 0;

        c += sprintf(buf+c, "x: %6.2f m/s^2\n", accX); buf[c - 10] = '.';
        c += sprintf(buf+c, "y: %6.2f m/s^2\n", accY); buf[c - 10] = '.';
        c += sprintf(buf+c, "z: %6.2f m/s^2\n", accZ); buf[c - 10] = '.';

        myOled.setCursor(0, 0); //@ Set the print cursor to the initial location
        myOled.writeString(buf);
        
        printLineBreak(myOled);
        printRawData(myOled, addr, data, len);
    }
}*/




