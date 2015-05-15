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

int bt_init(const char* name)
{
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

void master(const char* addr_str)
{
  // Choose device and print Address
  setColor(R);
  n += sprintf(buf+n, "%s\n", addr_str);
  flush();

  // Connect
  ret[4] = WyzBee_SPPConnet((uint8_t*)addr_str);
  flush();

  ret[5] = WaitForSPPConnComplete();
  flush();
  setColor(OFF);
}

void slave()
{
  // Connect
  setColor(R);
  ret[5] = WaitForSPPConnComplete();
  flush();
  setColor(OFF);
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
