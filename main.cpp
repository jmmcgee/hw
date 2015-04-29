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
#include "game.h"

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
  intro();
  while(1)
  {
    c = 0;
    readyByte();
    //if(!!WyzBeeGpio_Get(4E))
    //  continue;

    setColor(G_OFF);

    char key = 0;
    int block = 2;
    if(block == 0 && bytesReady > 3)
    {
      setColor(R);

      c += sprintf(buf+c, "br: %d\n", bytesReady);
      for(int i = 0; i < 12 && i < bytesReady; i++) {
        c += sprintf(buf+c, "%02x ", bytes[bytePos+i]);
        if((i % 4) == 3)
          c += sprintf(buf+c, "\n"); 
      }
      c += sprintf(buf+c, "\n"); 
      key = readInput();
      c += sprintf(buf+c, "key: %18c\n", key);
      c += sprintf(buf+c, "br: %d\n", bytesReady);
      c += sprintf(buf+c, "---------------------\n", bytesReady);

      //if(oled.getCursorY() > 128)
      oled.fillScreen(BLACK); //@ fills the OLED screen with black pixels
      oled.setCursor(0, 0);
      oled.writeString(buf,c);
      for(int i = 0; i < 100; i++);
      setColor(R_OFF);
    }

    else if(block == 1)
    {
      for(int i =0; i < 4; i++) {
        c += sprintf(buf+c, "%02x", nextByte());
        c += sprintf(buf+c, (i < 3 ? " " : " (%3d)\n"), pos);
      }

      oled.writeString(buf,c);
    }

    else if(block == 2)
    {
      game();
    }

    setColor(G);
    //delay(500);
  }
}







