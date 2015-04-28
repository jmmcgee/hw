#include <stdio.h>
#include <string.h>

#include <Adafruit_GFX.h>
#include "Adafruit_SSD1351.h"
#include <SPI_OLED.h>
#include <WyzBee_spi.h>
#include <WyzBee_i2c.h>
#include <WyzBee_gpio.h>

extern Adafruit_SSD1351 tft = Adafruit_SSD1351(); //@  OLED class variable
/*===================================================*/
/**
 * @fn            void print_oled(int8 *text, uint16_t color)
 * @brief        this functions is used to print the data on the oled screen
 * @param 1[in]    int8 *text
 * @param 2[in]    uint16_t color , color
 * @param[out]    none
 * @return        none
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

void setColor(int color);

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

    WyzBeeGpio_InitIn(10,  0);
    WyzBeeGpio_InitOut(12,  1);

    char buf[128];
    int c = 0;

    uint8_t ir;
    while(1) {
        ir = WyzBeeGpio_Get(10);
        WyzBeeGpio_Put(GPIO_2, ir); //GPIO_2 = PIN 4 = PORT P12
        setColor(!ir);
    }
}


void setColor(int color)
{
    WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_HIGH);
    WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_HIGH);
    WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_HIGH);
    int ms=50;
    for(int i=0; i < 100*ms; i++);
    switch(color) {
        case 0:
            /*Toggling Red LED*/
            WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_LOW);
            break;

        case 1:
            /*Toggling Green LED*/
            WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_LOW);
            break;
        
        case 2:
            /*Toggling Blue LED*/
            WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
            break;
        
        // SHOULD NEVER HAPPEN
        default:
            WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_LOW);
            WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_LOW);
            WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
    }
    for(int i=0; i < 100*ms; i++);
}

