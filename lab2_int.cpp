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

volatile uint32_t intervals[NUM_INTERVALS] = {0};
volatile uint32_t lastInterval = 0;
volatile uint32_t lastTime = 0;
volatile uint32_t pos = 0;

volatile uint8_t bytes[NUM_BYTES] = {0};
volatile uint32_t bytePos = 0;
volatile uint32_t bytesReady = 0;

volatile char key = 0;

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

  err = Dt_Init(&dt_Internal,Dt_Channel0);
  err = Dt_EnableCount(Dt_Channel0);
}

void extInt()
{
  static volatile uint8_t flag;

  if(!flag)
  {
    setColor(B);

    uint32_t time = Dt_ReadCurCntVal(Dt_Channel0);
    intervals[lastInterval] = time < lastTime ? lastTime - time: lastTime + (4294967295u - time);
    lastTime = time;

    if(++lastInterval >= NUM_INTERVALS)
      lastInterval -= NUM_INTERVALS;
    flag=1;
  }
  else
  {
    setColor(B_OFF);
    flag = 0;
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


// this function separates our code into intervals
// this way the stream of code is easier to separate and decode 
// from the incoming signal
interval_t interperetInterval(uint32_t interval)
{
  interval_t val = LONG;
  double ratio = (double)interval / 12000.0;
  double tolerance = 0.1;

  if(ratio > (1.0 - tolerance) && ratio < (1.0 + tolerance))
    val = LOW;
  else if(ratio > (1.5 - tolerance) && ratio < (1.5 + tolerance))
    val = HIGH;
  else if(ratio > (2.0 - tolerance) && ratio < (2.0 + tolerance))
    val = BREAK;
  return val;
}

// gives us the next bit to use
interval_t nextBit()
{
  if(pos == lastInterval)
    return NOT_READY;
  interval_t bit = interperetInterval(intervals[pos]);

  if(++pos >= NUM_INTERVALS)
    pos -= NUM_INTERVALS;
  return bit;
}


// this function is readybyte, and it check to see if there is code 
// sorts it accordingly to the intervals provided in the other section
// in main, this function is used to determine if there is a signal to be read
int readyByte()
{
  interval_t bit = LONG;
  uint8_t code = bytes[bytePos];

  // read code
  while(1) {
    if( (bit = nextBit()) == LONG)
      break;
    else if(bit == LOW)
      code = (code << 1) | 0x00000000;
    else if(bit == HIGH)
      code = (code << 1) | 0x00000001;
    else if(bit == NOT_READY) {
      bytes[bytePos] = code;
      return 0;
    }
  } // read code

  bytes[(bytePos+bytesReady)%NUM_BYTES] = code;
  bytesReady++;
  return 1;
}


// gives us the next byte to work with
uint8_t nextByte()
{
  if(bytesReady > 0) {
    if(++bytePos >= NUM_BYTES)
      bytePos -= NUM_BYTES;
    bytesReady--;
    return bytes[bytePos];
  }
  else
    return -1;
}

// this is the statement that actually checks to see if 
//the hex code matches our human-understood numbers
// originally, this code was supposed to match HEX --> DEC by our own definition
// however, there were issues because the code was not always consistent. 

// so, here we flip the bits and divide it into two chunks to compare
// byte1 and byte2
// if we receive byte 1, we check to see if byte 2 follows. 
// if not, continue checking, if yes, set value

// this is done for every button we decode (0123456789)
// see section "WHAT OUR CODE DOES" for details!
char readInput()
{
  char val = 0;
  int count = 1;
  uint8_t byte1, byte2;

  byte1 = nextByte();
  byte2 = nextByte();

SWITCH:
  switch(byte1)
  {
    case 0x28:
      switch(byte2)
      {
        case 0x13:
          val = '7';
          break;
      }
      break;


    case 0x2a:
      switch(byte2)
      {
        case 0x49:
          val = '8';
          break;
      }
      break;

    case 0x49:
      switch(byte2)
      {
        case 0x2a:
          val = '8';
          break;
      }
      break;

    case 0x50:
      switch(byte2)
      {
        case 0x10:
          val = '1';
          break;
      }
      break;

    case 0x52:
      switch(byte2)
      {
        case 0x91:
          val = '5';
          break;

        case 0x92:
          val = '3';
          break;
      }
      break;

    case 0x54:
      switch(byte2)
      {
        case 0x12:
          val = '2';
          break;
      }
      break;


    case 0x56:
      switch(byte2)
      {
        case 0xc8:
          val = '4';
          break;

        case 0x93:
          val = '6';
          break;
      }
      break;

    case 0x58:
      switch(byte2)
      {
        case 0x18:
          val = '9';
          break;
      }
      break;

    case 0x5c:
      switch(byte2)
      {
        case 0x1a:
          val = '0';
          break;
      }
      break;
    
    case 0xd2:
      switch(byte2)
      {
        case 0x91:
          val = '5';
      }
      break;
 
    case 0xd6:
      switch(byte2)
      {
        case 0x93:
          val = '6';
      }

    case 0xe8:
      switch(byte2)
      {
        case 0x13:
          val = '7';
      }
  } // switch code

	// go back and check again, 
	// goto SWITCH statement when done checking first round
  if(val == 0 && count) {
    byte1 = byte2;
    byte2 = nextByte();
    count--;
    goto SWITCH;
  }

	// return val!
  return val;
}
