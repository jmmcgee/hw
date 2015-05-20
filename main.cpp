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
char lastKey = 0;
uint8_t ir;

int main(void)
{
	// initialization statements
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
    int block = 2; // "block" of hex code
    
    // continuation: here we read in the bytes in hex format
    // this is how we check the pattern of the infared signal (we don't use the logic analyzer here)
    
    // diagnose with hex
    // we read in the signal bytesReady
    if(block == 0 && bytesReady > 3)
    {
    	// reading in a block/byte
    	// set LED color to red, reading in (do not touch!)
    	// we used the LED to help us with some basic debugging
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
      oled.setCursor(0, 0); // sets cursor to beg
      oled.writeString(buf,c); // writes string
      for(int i = 0; i < 100; i++); // serves as a delay statement
      setColor(R_OFF); // turn RED off
    }

    // print hex 
    // note: there is room for error here, since the infared detector and signal is not always accurate
    // the following two if statements check the "block" of hex code
    // there is a nested statement inside to switch and double check the other blocks
    // to make sure that the hex code read in matches the pattern accurately.
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
      if(bytesReady > 3) {
        key = readInput();
      }
      if(key == 0 || key == ' ' || key == lastKey)
        continue;
      lastKey = key;

      oled.setCursor(0,0);
      oled.write(key);
    }

	// when done, set color back to green (ready to go)
    setColor(G);
    //delay(500);
  }
}







