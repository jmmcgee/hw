#include <stdlib.h>
#include <string.h>

#include <delay.h>
#include <WyzBee_gpio.h>
#include <WyzBee_bt.h>
#include <WyzBee_spi.h>

#include "lab2_int.h"

int main()
{
  initOled();
  sys_ticks_init();
  WyzBee_BT_init();

  char* buf = new char[336];
  int n = 0;
  
  buf = "OLED workds";
  n = strlen(buf);
  
  oled.writeString(buf, n);
  WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_LOW);
}
