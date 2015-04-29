#include "game.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <WyzBee.h>
#include <WyzBee_kit.h>

#include <WyzBee_gpio.h>
#include <WyzBee_ext.h>
#include <WyzBee_timer.h>
#include <WyzBee_spi.h>
#include <SPI_OLED.h>


#include "lab2_int.h"
#include "game.h"
static char buf[336];
static int c =0;
void intro()
{
}

void game()
{
  static int state = 0;
  char key = 0;
  switch(state) {
    case 0:
      c = 0;
      c += sprintf(buf+c, "Let's play a game.  Guess a number from 0 to 9...\n");
      oled.fillScreen(BLACK); //@ fills the OLED screen with black pixels
      oled.setCursor(0,0);
      oled.writeString(buf, c);

      do {
        readyByte();
        key = readInput();
      } while(bytesReady < 3 || key == 0);

      state = 1;
      break;

    case 1:
      c = 0;
      c += sprintf(buf+c, "Your Guess: ");

      oled.fillScreen(BLACK); //@ fills the OLED screen with black pixels
      oled.setCursor(0,0);
      oled.writeString(buf, c);
      state = 2;
      break;

    case 2:
      c = 0;
//      while(!readyByte()  || bytesReady < 3 || key == 0) {
//        key = readInput();
//      }
      if(bytesReady < 3)
        return;
      key = readInput();
      if(key == 0) 
        return;

      c += sprintf(buf+c, "%c\n", key);
      oled.writeString(buf, c);
      state = 3;
      delay(100);
      break;

    case 3:
      c = 0;
      int max = 9;
      int num = ((RAND_MAX - rand())/(float)RAND_MAX)*max;
      c += sprintf(buf+c, "The answer was %d.\n", num);
      oled.writeString(buf, c);
      do {
        readyByte();
        key = readInput();
      } while(bytesReady < 3 || key == 0);

        state = 0;
      delay(100);
      break;

  }
}

void game2()
{
  static int16_t xpos = 64, ypos = 64;
  int16_t last_xpos = xpos, last_ypos = ypos;

  switch(key)
  {
    case '1':
      xpos -= 1;
      ypos -= 1;
      break;
    case '2':
      ypos -= 1;
      break;
    case '3':
      xpos += 1;
      ypos -= 1;
      break;
    case '4':
      xpos -= 1;
      break;
    case '5':
      break;
    case '6':
      xpos += 1;
      break;
    case '7':
      xpos -= 1;
      ypos += 1;
      break;
    case '8':
    case '0':
      ypos += 1;
      break;
    case '9':
      xpos += 1;
      ypos += 1;
      break;

    default:
      break;
  }
  drawPixel(xpos,  ypos, WHITE);
  drawPixel(last_xpos, last_ypos, BLACK);
  key = ' ';
}
