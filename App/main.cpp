#include <stdlib.h>
#include <string.h>

#include <delay.h>
#include <WyzBee_gpio.h>
#include <WyzBee_bt.h>
#include <WyzBee_spi.h>

#include "lab2_int.h"


char* buf = new char[336];
int n = 0;
int main()
{
  initOled();
  sys_ticks_init();
  WyzBee_BT_init();
	WyzBee_SetLocalName((uint8_t*)"jmmcgee-071");
	
	char* name = new char[32];
	WyzBee_GetLocalName((uint8_t*)name);
  n = strlen(name);
  
  oled.writeString(name, n);
}
