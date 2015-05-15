#include "main.h"

#include <WyzBee_gpio.h>
#include <WyzBee_bt.h>

#include "bt.h"
#include "remote.h"

int n = 0;
char buf[336] = {0};
uint8_t data[128] = {0};
uint16_t data_len = 0;
const char* master_str = "00:23:A7:80:59:ED"; // 71, master
const char* slave_str = "00:23:A7:80:59:F9"; // 990, slave

int main()
{
  initOled();

  bool isMaster = 1;
  bool hasBT = 1;
  bool hasIR = 0;


  if(hasIR) {
    initExtInt();
    initTimer();
  }

  if(hasBT && isMaster) {
    bt_init("jmm-master");
    master();

    data[0] = 0;
    while(1) {
      setColor(ON);
      data_len = 1;
      WyzBee_SPPTransfer((uint8_t*)slave_str, &data[0], data_len);
      data[0]++;
      setColor(OFF);
    }
  }// master
  else if(hasBT && !isMaster) {
    bt_init("jmm-slave");
    slave();

    data[0] = 0;
    while(1) {
      while(!!WyzBeeGpio_Get(4E));
      setColor(R);
      data_len = 1;
      WyzBee_SPPReceive(data, data_len);
      oled.setCursor(0,8*5);
      oled.writeString((char*)data, data_len);
      setColor(OFF);
    }
  } // slave
}
