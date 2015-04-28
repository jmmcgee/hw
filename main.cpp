/*
 * Includes
 */
#include <stdio.h>
#include <string.h>

#include <WyzBee.h>
#include <WyzBee_kit.h>

#include <WyzBee_gpio.h>
#include <WyzBee_ext.h>
#include <WyzBee_timer.h>
#include <WyzBee_spi.h>
#include <SPI_OLED.h>


#include "lab2_int.h"

#define NCOLS 21
#define NROWS 16

int screen = 0;
char buf[336];
int c = 0;
char val = 0;
uint8_t ir;

int main(void)
{
  initExtInt();
  initOled();
  initTimer();
  WyzBeeGpio_InitIn(10, 0);
  WyzBeeGpio_InitOut(12, 1);

  oled.setCursor(0,0);
  setColor(G);
  screen = 0;
  while(1)
  {
    if(!!WyzBeeGpio_Get(4E))
      continue;

    setColor(G_OFF);
    setColor(R);

    // switch code
    if(pos + 1 >= lastInterval)
      continue;

    switch(nextByte())
    {
      case 0x28:
        switch(nextByte())
        {
          case 0x00:
            switch(nextByte())
            {
              case 0x13:
                val = '7';
                break;
            }
            break;
        }
        break;


      case 0x2a:
        switch(nextByte())
        {
          case 0x00:
            switch(nextByte())
            {
              case 0x09:
                val = '8';
                break;
            }
            break;
        }
        break;

      case 0x50:
        switch(nextByte())
        {
          case 0x00:
            switch(nextByte())
            {
              case 0x10:
                val = '1';
                break;
            }
            break;
        }
        break;

      case 0x52:
        switch(nextByte())
        {
          case 0x00:
            switch(nextByte())
            {
              case 0x11:
                val = '5';
                break;

              case 0x12:
                val = '3';
                break;
            }
            break;
        }
        break;

      case 0x54:
        switch(nextByte())
        {
          case 0x00:
            switch(nextByte())
            {
              case 0x12:
                val = '2';
                break;
            }
            break;
        }
        break;


      case 0x56:
        switch(nextByte())
        {
          case 0x00:
            switch(nextByte())
            {
              case 0x08:
                val = '4';
                break;

              case 0x13:
                val = '6';
                break;
            }
            break;
        }
        break;

      case 0x58:
        switch(nextByte())
        {
          case 0x00:
            switch(nextByte())
            {
              case 0x18:
                val = '9';
                break;
            }
            break;
        }
        break;

      case 0x5c:
        switch(nextByte())
        {
          case 0x00:
            switch(nextByte())
            {
              case 0x1a:
                val = '0';
                break;
            }
            break;
        }
        break;
    } // switch code

    //key_t key = readInput();
    if(pos + 1 >= lastInterval)
      uint8_t code = nextByte();
    
    val = readInput();
    if(val != -1)
      c += sprintf(buf+c, "%c ", val);

   if(pos + 4 >= lastInterval)
    continue;

//   for(int i =0; i < 4; i++) {
//    c += sprintf(buf+c, "%02x", nextByte());
//    c += sprintf(buf+c, (i < 3 ? " " : " (%3d)\n"), pos);
//   }


    //oled.setCursor(0, 0);
    
    //if(oled.getCursorY() > 128)
    oled.setCursor(0, 0);
    oled.writeString(buf,c);
    c = 0;

    setColor(R_OFF);
    setColor(G);
    delay(500);
  }
}







