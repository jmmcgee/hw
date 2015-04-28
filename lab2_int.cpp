#include "lab2_int.h"

#include "./common/base_types.h"


#include <WyzBee_kit.h>
#include <WyzBee.h>

#include <WyzBee_gpio.h>
#include <WyzBee_ext.h>
#include <WyzBee_timer.h>
#include <WyzBee_spi.h>
#include <SPI_OLED.h>

#include <timetick.h>


volatile uint32_t timerCount = 0;
volatile uint32_t extIntCount = 0;
volatile uint32_t intervals[NUM_INTERVALS] = {0};
volatile uint32_t lastInterval = 0;
volatile uint32_t lastTime = 0;

static WyzBee_exint_config_t WyzBeeExtIntConfig;

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

int initExtInt()
{
  extIntCount  = 0;

  //setup an IR interrupt
  WyzBee_PDL_ZERO(WyzBeeExtIntConfig);                      // zero struct
  WyzBeeExtIntConfig.abEnable[EXT_PORT] = FALSE;            // INT2 ???
  WyzBeeExtIntConfig.aenLevel[EXT_PORT] = ExIntFallingEdge;  // set part of signal to detect
  WyzBeeExtIntConfig.apfnExintCallback[EXT_PORT] = &extInt; // set callback
  WyzBee_Exint_IR_Init(&WyzBeeExtIntConfig); // init IR interrupt
  WyzBee_Exint_EnableChannel(EXT_PORT);
}


int initTimer()
{
  err_t err;
  stc_dt_channel_config_t  dt_Internal;

  dt_Internal.u8Mode = Dt_FreeRun;
  dt_Internal.u8PrescalerDiv = Dt_PrescalerDiv16;
  dt_Internal.u8CounterSize =  Dt_CounterSize32;

  timerCount = 0;
  err = Dt_Init(&dt_Internal,Dt_Channel0);
	err = Dt_EnableCount(Dt_Channel0);
}

void extInt()
{
  static uint8_t flag;

  if(!flag)
  {
    setColor(B);

    uint32_t time = Dt_ReadCurCntVal(Dt_Channel0);
    intervals[lastInterval] = time < lastTime ? lastTime - time: lastTime + (4294967295u - time);
		lastTime = time;

    extIntCount++;
    if(lastInterval < NUM_INTERVALS)
        ++lastInterval;
    flag=1;
  }
  else
  {
    setColor(B_OFF);
    flag =0;
  }
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
        Wait(delay);
        WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_HIGH);
      }
      break;

    case G:
      /*Toggling Green LED*/
      WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_LOW);
      if(delay > 0) {
        Wait(delay);
        WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_HIGH);
      }
      break;

    case B:
      /*Toggling Blue LED*/
      WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
      if(delay > 0) {
        Wait(delay);
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

interval_t interperetInterval(uint32_t interval)
{
	interval_t val = LONG;
	float ratio = (float)interval / 12000.0;
	float tolerance = 0.1;
	
	if(ratio > (1.0 - tolerance) && ratio < (1.0 + tolerance))
		val = LOW;
	else if(ratio > (1.5 - tolerance) && ratio < (1.5 + tolerance))
		val = HIGH;
	else if(ratio > (2.0 - tolerance) && ratio < (2.0 + tolerance))
		val = BREAK;
	return val;
}