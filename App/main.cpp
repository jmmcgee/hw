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
	char buf[336] = {0};	
	RSI_BT_EVENT_INQUIRY_RESPONSE* res;
	uint8 size = 5;
	res = new RSI_BT_EVENT_INQUIRY_RESPONSE[5];
	int ret[10] = {0};

  initOled();
  sys_ticks_init();
  WyzBee_BT_init();
	WyzBee_SetLocalName((uint8_t*)"jmmcgee-071");

	// Get Name
	WyzBee_GetLocalName((uint8_t*)buf);
  n = strlen(buf);

	// Set Discoverable, Connectable, Init
	ret[0] = WyzBee_SetDiscoverMode(1, 500);
	ret[1] = WyzBee_SetConnMode(1);
	ret[2] = WyzBee_InitSppProfile();
	
	setColor(R);

	// Find Devices
	ret[3] = WyzBee_GetInquiryResults(res, size);

	setColor(R_OFF);
	setColor(G);
	
	// Print Available Devices
	n += sprintf(buf+n, "\n");
	for(int i = 0; i < size; i++) {
		n += sprintf(buf+n, "%d) %s\n", i, res->RemoteDeviceName);
	}

	setColor(G_OFF);
	setColor(B);
	
	// Choose device and print Address
	int deviceNum = 0;
	uint8_t bt_str[18] = {0};
	BT_BDAddrToStr((res+deviceNum)->BDAddress, bt_str);
	n += sprintf(buf+n, "%s\n", bt_str);

	setColor(B_OFF);
	setColor(ON);

	// Connect
	ret[4] = WyzBee_SPPConnet(bt_str);
	ret[5] = WaitForSPPConnComplete();

	setColor(OFF);

	// Print return values
	n += sprintf(buf+n, "\nret:");
	for(int i = 0; i < 5; i++)
    n += sprintf(buf+n, " %d", ret[i]);
	n += sprintf(buf+n, "\n");
	
	oled.writeString(buf, n);
}
