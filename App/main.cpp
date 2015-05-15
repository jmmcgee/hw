#include "main.h"

#include <WyzBee_gpio.h>
#include <WyzBee_bt.h>

#include <delay.h>

#include "bt.h"
#include "remote.h"

int n = 0;
char buf[336] = {0};
uint8_t data[128] = {0};
uint16_t data_len = 0;

#define BT_067 "00:23:A7:80:59:F9";
#define BT_071 "00:23:A7:80:59:ED"

const char* intro = "Select which char\nto send\nUse 0 or 2 to change\nPress 5 to confirm\n";

int main()
{
  initOled();

  bool hasIR = 1;
  bool hasBT = 1;
  bool isMaster = !!WyzBeeGpio_Get(4E);
  uint16_t ret;

  if(hasIR) {
    initExtInt();
    initTimer();
  }

  if(!hasBT)
    while(1)
      updateIR();

  if(isMaster) {
    bt_init("jmm-master");
    const char* target_addr_str = BT_067;
    master(target_addr_str);

    //initialize screen
    n += sprintf(buf+n, "%s", intro);
    flush(0);

    printKey = 'A';
    uint16_t xPos = oled.getCursorX();
    uint16_t yPos = oled.getCursorY();
    oled.setCursor(128-6,8*15);
    oled.write(printKey);
    oled.setCursor(xPos, yPos);


    data[0] = 0;
    while(1) {
      setColor(G);
      while(!updateIR());

      data[0] = printKey;
      data_len = 1;
      ret = WyzBee_SPPTransfer((uint8_t*)target_addr_str, &data[0], data_len);

      //n += sprintf(buf+n, "%ret: %4d\n", ret);
      //n += sprintf(buf+n, "%c : %3d\n", data[0], data[0]);
      flush(8*5);

      //data[0]++;
      setColor(G_OFF);
      delay(1000);
    }
  }// master
  else if(!isMaster) {
    bt_init("jmm-slave");
    slave();

    data[0] = 0;
    while(1) {
      while(!!WyzBeeGpio_Get(4E));
      setColor(G);
      data_len = 1;
      ret = WyzBee_SPPReceive(data, data_len);

      n += sprintf(buf+n, "%ret: %4d\n", ret);
      n += sprintf(buf+n, "%c : %3d\n", data[0], data[0]);
      flush(8*5);

      setColor(G_OFF);
      delay(500);
    }
  } // slave
} // */

