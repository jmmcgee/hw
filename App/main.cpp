#include <stdlib.h>
#include <string.h>

#include <delay.h>
#include <WyzBee_gpio.h>
#include <WyzBee_bt.h>
#include <WyzBee_spi.h>

#include "lab2_int.h"


int n = 0;
int main()
{
  initOled();
  sys_ticks_init();
  WyzBee_BT_init();
	WyzBee_SetLocalName((uint8_t*)"jmmcgee-071");
	
	char buf[336] = {0};
	
	int ret[4] = {0};
	ret[0] = WyzBee_SetDiscoverMode(1, 1000);
	ret[1] = WyzBee_SetConnMode(1);
	ret[2] = WyzBee_InitSppProfile();
	
	RSI_BT_EVENT_INQUIRY_RESPONSE p_scan_res;
	uint8 size = sizeof(p_scan_res);

	//ret[3] = WyzBee_GetInquiryResults(&p_scan_res, size);

	WyzBee_GetLocalName((uint8_t*)buf);
  n = strlen(buf);
	
	n += sprintf(buf+n, "\nret:");
	for(int i = 0; i < 4; i++)
		n += sprintf(buf+n, " %d", ret[i]);
	n += sprintf(buf+n, "\n");
	oled.writeString(buf, n);

	//WyzBee_SetConnMode
	//WyzBee_InitSppProfile
	//WyzBee_GetInquiryResults
  
}
