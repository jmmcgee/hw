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


int main(void)
{
  char buf[336];
  int c = 0;
	uint8_t ir;

  initExtInt();
  initOled();
  initTimer();
	WyzBeeGpio_InitIn(10,  0);
	WyzBeeGpio_InitOut(12,  1);

  c += sprintf(buf+c, "ext: %3d\n", extIntCount);
  c += sprintf(buf+c, "val: %3d\n", Dt_ReadCurCntVal(Dt_Channel0));
  for(int i = 0; i < NUM_INTERVALS; i++)
    c += sprintf(buf+c, "%2d: %5d     \n", i, intervals[(lastInterval+i)%NUM_INTERVALS]);

  oled.setCursor(0,0);
  oled.writeString(buf, c);
  c = 0;
	
	setColor(G);
  while(1)
  {
		ir = WyzBeeGpio_Get(10);
		WyzBeeGpio_Put(GPIO_2, ir); //GPIO_2 = PIN 4 = PORT P12

    if(!!WyzBeeGpio_Get(4E))
      continue;

		setColor(G_OFF);
		setColor(R);
    c += sprintf(buf+c, "ext: %3d\n", extIntCount);
    c += sprintf(buf+c, "val: %3d\n", Dt_ReadCurCntVal(Dt_Channel0));
    for(int i = 0; i < NUM_INTERVALS; i++)
      c += sprintf(buf+c, "%2d: %4d\n", i, intervals[(lastInterval+i)%NUM_INTERVALS]);

    oled.setCursor(0,0);
    oled.writeString(buf, c);
    c = 0;
		setColor(R_OFF);
		setColor(G);
  }

}







