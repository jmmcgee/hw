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
char key = 0;
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
    c = 0;
    if(!!WyzBeeGpio_Get(4E))
      continue;

    setColor(G_OFF);
    setColor(R);

    char key = 0;
    if(0)
    {
      key = readInput();

      c += sprintf(buf+c, "%c ", key);
      oled.setCursor(0, 0);
      oled.writeString(buf,c);
    }
    

    {
      for(int i =0; i < 4; i++) {
        c += sprintf(buf+c, "%02x", nextByte());
        c += sprintf(buf+c, (i < 3 ? " " : " (%3d)\n"), pos);
      }
      //c += sprintf(buf+c, "%02x\n", nextByte());

      oled.writeString(buf,c);
    }

    setColor(R_OFF);
    setColor(G);
    //delay(500);
  }
}







