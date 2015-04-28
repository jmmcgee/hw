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
    key_t key = readInput();
    switch(key) {
      case KEY_0:
        val = '0';
        break;
      case KEY_1:
        val = '1';
        break;
      case KEY_2:
        val = '2';
        break;
      case KEY_3:
        val = '3';
        break;
      case KEY_4:
        val = '4';
        break;
      case KEY_5:
        val = '5';
        break;
        /*case KEY_6:
          val = '6';
          break;*/
      case KEY_7:
        val = '7';
        break;
      case KEY_8:
        val = '8';
        break;
      case KEY_9:
        val = '9';
        break;

      case NONE:
      default:
        val = '*';
    } // switch code

    oled.writeString(&val, 1);
    c = 0;

    setColor(R_OFF);
    setColor(G);
  }
}







