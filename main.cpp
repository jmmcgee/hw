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
uint8_t ir;

int main(void)
{
  initExtInt();
  initOled();
  initTimer();
  WyzBeeGpio_InitIn(10, 0);
  WyzBeeGpio_InitOut(12, 1);

  setColor(G);
  screen = 0;
  while(1)
  {
    ir = WyzBeeGpio_Get(10);
    WyzBeeGpio_Put(GPIO_2, ir); //GPIO_2 = PIN 4 = PORT P12

    if(!!WyzBeeGpio_Get(4E))
      continue;

    setColor(G_OFF);
    setColor(R);
    //c += sprintf(buf+c, "ext: %3d\n", extIntCount);
    //c += sprintf(buf+c, "val: %3d\n", Dt_ReadCurCntVal(Dt_Channel0));

    for(int row = 0; row < NROWS; row++) {
			for(int bit = 0; bit < 8; bit++) {
				int index = row*16 + 0 + bit;
				if(index > NUM_INTERVALS)
					break;
				else
					c += sprintf(buf+c, "%1d", interperetInterval(intervals[row*16 + 8 + bit]));
			}
			
			c += sprintf(buf+c, " ");

			for(int bit = 0; bit < 8; bit++) {
				int index = row*16 + 8 + bit;
				if(index > NUM_INTERVALS)
					break;
				else
					c += sprintf(buf+c, "%1d", interperetInterval(intervals[row*16 + 8 + bit]));
			}
			
			c += sprintf(buf+c, "\n");
		}

    ++screen %= NUM_INTERVALS/NROWS;
 
    oled.setCursor(0,0);
    oled.writeString(buf, c);
    c = 0;

    setColor(R_OFF);
    setColor(G);
  }

}







