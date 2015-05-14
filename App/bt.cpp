#include "bt.h"

#include <stdlib.h>
#include <string.h>

#include <delay.h>
#include <WyzBee_gpio.h>
#include <WyzBee_bt.h>
#include <WyzBee_spi.h>
#include <timetick.h>

#include "main.h"
#include "remote.h"

#define NUM_DEVICES 5

RSI_BT_EVENT_INQUIRY_RESPONSE res[NUM_DEVICES];

int ret[10] = {9};
uint8_t data[128] = {0};
uint16_t data_len = 0;

int bt_init(const char* name)
{
  initOled();
  sys_ticks_init();
  WyzBee_BT_init();
  WyzBee_SetLocalName((uint8_t*)name);

  // Get Name and Local Address
  WyzBee_GetLocalName((uint8_t*)buf+n);
  n = strlen(buf);
  n += sprintf(buf+n, "\n");

  WyzBee_GetLocalBDAddress((uint8_t*)buf+n);
  n = strlen(buf);
  n += sprintf(buf+n, "\n");
  flush();

  // Set Discoverable, Connectable, Init
  ret[0] = WyzBee_SetDiscoverMode(1, 500);
  ret[1] = WyzBee_SetConnMode(1);
  ret[2] = WyzBee_InitSppProfile();
  flush();
}

void master()
{
  setColor(R);

  // Choose device and print Address
  n += sprintf(buf+n, "%s\n", slave_str);
  flush();

  setColor(R_OFF);
  setColor(B);

  // Connect
  ret[4] = WyzBee_SPPConnet((uint8_t*)slave_str);
  flush();

  setColor(B_OFF);
  setColor(ON);

  ret[5] = WaitForSPPConnComplete();
  flush();

  data[0] = 0;
  while(1)
  {
    setColor(ON);
    data_len = 1;
    WyzBee_SPPTransfer((uint8_t*)slave_str, &data[0], data_len);
    data[0]++;
    setColor(OFF);
  }

  setColor(OFF);
}

void slave()
{
  setColor(R);

  // Choose device and print Address
  n += sprintf(buf+n, "%s\n", master_str);
  flush();

  setColor(R_OFF);
  setColor(B);

  // Connect
  //ret[4] = WyzBee_SPPConnet(master_str);
  //flush();

  setColor(B_OFF);
  setColor(ON);

  ret[5] = WaitForSPPConnComplete();
  flush();

  setColor(OFF);

  while(1)
  {
    while(!!WyzBeeGpio_Get(4E));
    setColor(ON);
    data_len = 1;
    WyzBee_SPPReceive(data, data_len);
    oled.setCursor(0,8*5);
    oled.writeString((char*)data, data_len);
    setColor(OFF);
  }
}

void printDevices()
{
  // Print Available Devices
  for(int i = 0; i < NUM_DEVICES; i++) {
    n += sprintf(buf+n, "%d) %s\n", i, res->RemoteDeviceName);
  }
}

void printRetVals()
{
  // Print return values
  n += sprintf(buf+n, "ret:");
  for(int i = 0; i < 5; i++)
    n += sprintf(buf+n, " %d", ret[i]);
}

void flush()
{
  printRetVals();
  oled.setCursor(0,oled.getCursorY());
  oled.writeString(buf, n);
  n = 0;
}
