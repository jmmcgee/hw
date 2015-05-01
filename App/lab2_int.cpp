#include "lab2_int.h"

#include <base_types.h>

#include <WyzBee_gpio.h>
#include <WyzBee_spi.h>
#include <SPI_OLED.h>

#include <timetick.h>

volatile uint32_t intervals[NUM_INTERVALS] = {0};
volatile uint32_t lastInterval = 0;
volatile uint32_t lastTime = 0;
volatile uint32_t pos = 0;

volatile uint8_t bytes[NUM_BYTES] = {0};
volatile uint32_t bytePos = 0;
volatile uint32_t bytesReady = 0;

volatile char key = 0;


Adafruit_SSD1351 oled = Adafruit_SSD1351(); //@  OLED class variable

  int initOled()
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

  WyzBeeSpi_Init(&config_stc); // initialize spi
  oled.begin(); //@ OLED screen Initialization

  // Prepare screen
  oled.fillScreen(BLACK); //@ fills the OLED screen with black pixels
  oled.setTextSize(1); //@ Set the text size on the OLED
  oled.setTextColor(WHITE, BLACK);
  return 0;
}

void setColor(uint8_t color, uint32_t delay)
{

  int ms=50;
  for(int i=0; i < 100*ms; i++);
  switch(color) {
    case R:
      /*Toggling Red LED*/
      WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_LOW);
      if(delay > 0) {
        WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_HIGH);
      }
      break;

    case G:
      /*Toggling Green LED*/
      WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_LOW);
      if(delay > 0) {
        WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_HIGH);
      }
      break;

    case B:
      /*Toggling Blue LED*/
      WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
      if(delay > 0) {
        WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_HIGH);
      }
      break;

    case ON:
      WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_LOW);
      WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_LOW);
      WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
      break;

    case R_OFF:
      WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_HIGH);
      break;

    case G_OFF:
      WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_HIGH);
      break;

    case B_OFF:
      WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_HIGH);
      break;   

    case OFF:
      WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_HIGH);
      WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_HIGH);
      WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_HIGH);
      break;
  }
  for(int i=0; i < 100*ms; i++);
}
