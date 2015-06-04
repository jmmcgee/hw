/*
 *********************************************************************************************************
 *
 *                        (c) Copyright 2015-2020 RedPine Signals Inc. HYDERABAD, INDIA
 *                                            All rights reserved.
 *
 *               This file is protected by international copyright laws. This file can only be used in
 *               accordance with a license and should not be redistributed in any way without written
 *               permission by Redpine Signals.
 *
 *                                            www.redpinesignals.com
 *
 *********************************************************************************************************
 */
 /*
 *********************************************************************************************************
 *
 *											  main file
 *
 * File           : main.cpp
 * Programmer(s)  :
 * Description    : IR interrupt
 *********************************************************************************************************
 * Note(s)		 :
 *********************************************************************************************************
 */
 
 /*
 * Includes
 */
#include <WyzBee_gpio.h>
#include <WyzBee_ext.h>
#include <WyzBee_timer.h>
#include <Adafruit_GFX.h>
//#include "Adafruit_SSD1351.h"
#include <SPI_OLED.h>
#include <WyzBee_spi.h>
#include <stdio.h>
//#include <WyzBee_kit.h>
#include <WyzBee.h>
#include <math.h>
//#include <WyzBee_bt.h>
#include <string.h>
#include <rsi_global.h>
#include <rsi_api.h>
#include <WyzBeeWiFi.h>
#include <variant.h>
#include <delay.h>
#include <WyzBee_i2c.h>

#include <remote.h>


//SPI Specification Initialization Block
WyzBeeSpi_Config_t  config_stc={
		4000000,
		SpiMaster,
		SpiSyncWaitZero,
		SpiEightBits,
		SpiMsbBitFirst,
		SpiClockMarkHigh,
		SpiNoUseIntrmode,
		NULL,                                                                
		NULL
};




#define ROW(y) (8*(y))
#define COL(y) (6*(y))


volatile uint8 fflag = 0;
volatile uint32_t time = 0;
rsi_scanInfo remote_dev[RSI_AP_SCANNED_MAX];
int8 scan_dev[11][34];
int8 *p_ssid_name;
int8 ix, nbr_scan_devs;
HttpRequest http_req;
extern Adafruit_SSD1351 oled = Adafruit_SSD1351(); //@  OLED class variable

#define DATA_LEN 1000
#define BUFF_LEN 256
#define MSG_LEN 50
#define NUM_RESULTS 50
#define MAX_WORD_SIZE 20

int8 message[MSG_LEN] = {0};
int8 data[DATA_LEN] = {0};
char word[MAX_WORD_SIZE] = "castle";
char results[NUM_RESULTS][MAX_WORD_SIZE] = {0};
char buf[BUFF_LEN];
int row =0;
int n =0;

int printToOLED(char* stringToPrint, int color, int cursorX, int cursorY);
int initWifi();
int ir_test();
int wifi_test();
int wifi_app();
int input();

int main(void)
{
  bool hasWifi = 1;
  bool hasIR = 0;
  err_t err;
  int16 status;
  
  sys_ticks_init();
  initTimer();
  initOled();
	printToOLED("It's alive", RED, COL(0), ROW(15));
  
  if(hasWifi) {
    initWifi();
    wifi_app();
  }

  return 0;
}

int initWifi() {
  int16 status = 0;
  status = WyzBeeWiFi_Init(); 
  if(status != 0) {
    n += sprintf(buf+n, "ERROR: WyzBeeWiFi_Init()->%d", status);
    printToOLED(buf, RED, COL(0), ROW(15));
    return 1;
  }
  printToOLED("Wifi initialized              ", BLACK, COL(0), ROW(15));
  delay(100);

  //@ this functions scans the AP's in vicinity and returns the number of networks scanned.
  //@ returns pointer to a string of each SSID scanned in the network.
  memset(scan_dev,'\0',sizeof(scan_dev));
  nbr_scan_devs = WyzBeeWiFi_ScanNetworks(remote_dev);
  for (ix = 0; ix < nbr_scan_devs; ix++) {
    //@ copying the all SSIDs into a local buffer
    memcpy (scan_dev[ix], remote_dev[ix].ssid, strlen ((const int8 *)remote_dev[ix].ssid)); 
  }
  printToOLED("Wifi scanned                  ", BLACK, COL(0), ROW(15));
  delay(100);

  //connect to access point
  status = WyzBeeWiFi_ConnectAccessPoint((int8 *)"Wireless Bra", (int8 *)"andpanties2"); 
  if(status != 0) {
    n += sprintf(buf+n, "ERROR: WyzBeeWiFi_ConnectAccessPoint()->%d", status);
    printToOLED(buf, RED, COL(0), ROW(15));
    return 1;
  }
  printToOLED("Wifi Connected                ", BLACK, COL(0), ROW(15));
  row = 1;
  delay(100);
}


int wifi_app() {
	//char urlbuf[] = "http://calbeedemo.appspot.com/greetings?msg=IRIS";
	char url[256] = "http://www.dictionaryapi.com/api/v1/references/collegiate/xml";
  int originalUrlLen = strlen(url);
  int urlLen = originalUrlLen;
  //char key[] = "3d2de58e-d485-4e88-a652-61ca72ec3ea6"; // thesaurus key
  char key[] = "651c6dbc-0d15-42b6-a15f-ecb1ec4f6c0d"; // dictionary key

	int16 status = 0;
  int num = 0;

  /** GET DATA and PRINT RESULTS y**/
  GET:
  urlLen = originalUrlLen;
  urlLen += sprintf(url+urlLen, "/%s?key=%s", word, key);
  //for(int i = 0; i < (urlLen / 20 + 1); i++)
   // printToOLED(url+i*20, 20, GREEN, 0, ROW(row++));
  status = WyzBeeWiFi_HttpGet((int8 *)url, &http_req, data, DATA_LEN);
  n = sprintf(buf, "status=%d, bytes=%d", status, strlen(data));
  printToOLED(buf, BLACK, COL(0), ROW(15));
  
  /** PARSE **/
  PARSE:
  num = 0;
  char* pch = strtok(data, "<");
  while( (pch = strtok(NULL, "<")) != NULL
      && pch - data < DATA_LEN)
  {
    if(pch[0] != 'd' || pch[1] != 't') continue;
    char* end = strtok(NULL, "<");
    pch = strtok(pch, ": ");
    while((pch = strtok(NULL, ": ")) != NULL && pch < end) {
      strncpy(results[num++], pch, MAX_WORD_SIZE);
    }
    break;
  }

  /** PRINT RESULTS **/
  PRINT:
  //row = 2;
  for(int i = 0; i < num; i++) {
    printToOLED(results[i], BLACK, COL(2), ROW(row++));
  }
//  n = sprintf(buf, "num=%d", num);
//  printToOLED(buf, BLACK, 0, ROW(row++));

  /** PRINT RAW DATA **/
  //printToOLED(data, BLACK, 0, ROW(row++));
  input();
}

int inputPos = 0;
bool inputSelect = 0;
int input() {
  inputSelect = 0;
  uint32_t t1 = 0, t2 = 0, interval;
  while(!inputSelect) {
    bool isMaster = !!WyzBeeGpio_Get(4E);
    while(WyzBeeGpio_Get(4E));
    t1 = Dt_ReadCurCntVal(Dt_Channel0);
    while(!WyzBeeGpio_Get(4E));
    t2 = Dt_ReadCurCntVal(Dt_Channel0);

    interval = t2 > t1 ? t2 - t1 : t2 + (4294967295u - t1);
    n = sprintf(buf, "interval=%d", interval);
    printToOLED(buf, BLACK, COL(0), ROW(14));
  }
}

//Insert function to print to OLED display (Lab1).
int printToOLED(char* stringToPrint, int color, int cursorX, int cursorY){
	//set color of text
	oled.setTextColor(color, WHITE);
	
	//get string lendth
	int stringLength = strlen(stringToPrint);
	//set the print cursor to the inital location
	oled.setCursor(cursorX,cursorY);
	//print the string to the screen moving the cursor each time
	for(int i=0; i < stringLength; i++){
			oled.setCursor(cursorX+6*i,cursorY); //set the print cursor to the inital location
			oled.write(stringToPrint[i]); //write a char to screen
	}
	
	return 0;
}

/*
 *********************************************************************************************************
 *                                           END
 *********************************************************************************************************
 */
