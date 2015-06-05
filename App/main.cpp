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
#include <timetick.h>
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
int16 status = 0;
err_t err;
extern Adafruit_SSD1351 oled = Adafruit_SSD1351(); //@  OLED class variable

#define DATA_LEN 10000
#define BUFF_LEN 256
#define MSG_LEN 50
#define NUM_RESULTS 100
#define MAX_WORD_SIZE 20

int8 message[MSG_LEN] = {0};

char buf[BUFF_LEN];
int row =0;
int n =0;

int8 data[DATA_LEN] = {0};
char word[MAX_WORD_SIZE] = "castle";
char results[NUM_RESULTS][MAX_WORD_SIZE] = {0};
int xCoord[NUM_RESULTS] = {0};
int yCoord[NUM_RESULTS] = {0};
int num = 0;
int choice = 0;

const char baseUrl[] = "http://www.dictionaryapi.com/api/v1/references/collegiate/xml";
char url[256];

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
  
  sys_ticks_init();
  initTimer();
  initOled();
	printToOLED("It's alive", RED, COL(0), ROW(15));
  
  if(hasWifi) {
    if(initWifi())
      return 1;
    wifi_app();
  }
  
  if(hasIR) {
    initExtInt();
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
//  delay(100);

  //@ this functions scans the AP's in vicinity and returns the number of networks scanned.
  //@ returns pointer to a string of each SSID scanned in the network.
  memset(scan_dev,'\0',sizeof(scan_dev));
  nbr_scan_devs = WyzBeeWiFi_ScanNetworks(remote_dev);
  for (ix = 0; ix < nbr_scan_devs; ix++) {
    //@ copying the all SSIDs into a local buffer
    memcpy (scan_dev[ix], remote_dev[ix].ssid, strlen ((const int8 *)remote_dev[ix].ssid)); 
  }
  printToOLED("Wifi scanned                  ", BLACK, COL(0), ROW(15));
//  delay(100);

  //connect to access point
  //status = WyzBeeWiFi_ConnectAccessPoint((int8 *)"Wireless Bra", (int8 *)"andpanties2"); 
  status = WyzBeeWiFi_ConnectAccessPoint("AndroidAP-jmmcgee", NULL); 
  if(status != 0) {
    n += sprintf(buf+n, "ERROR: WyzBeeWiFi_ConnectAccessPoint()->%d", status);
    printToOLED(buf, RED, COL(0), ROW(15));
    delay(1000);
    return (1);
  }
  printToOLED("Wifi Connected                ", BLACK, COL(0), ROW(15));
  row = 1;
//  delay(100);
  return 0;
}

int wifiAppGet() {
  /** GET DATA and PRINT RESULTS y**/

  //char urlbuf[] = "http://calbeedemo.appspot.com/greetings?msg=IRIS";

  //char key[] = "3d2de58e-d485-4e88-a652-61ca72ec3ea6"; // thesaurus key
  static const char key[] = "651c6dbc-0d15-42b6-a15f-ecb1ec4f6c0d"; // dictionary key

  int urlLen = strlen(baseUrl);
  strncpy(url, baseUrl, 256);
  urlLen += sprintf(url+urlLen, "/%s?key=%s", word, key);

  // print url and HALT
//  for(int i = 0; i < (urlLen / 20 + 1); i++)
//    printToOLED(url+i*20, GREEN, 0, ROW(row++));
//  setColor(ON);
//  while(WyzBeeGpio_Get(4E));
//  setColor(OFF);

  n = sprintf(buf, "Loading %s", url);
  printToOLED(buf, BLACK, COL(0), ROW(15));
  status = WyzBeeWiFi_HttpGet((int8 *)url, &http_req, data, 1000);
  n = sprintf(buf, "status=%d, bytes=%d", status, strlen(data));
  printToOLED(buf, BLACK, COL(0), ROW(15));
  return status;
}


int wifiAppParse() {
  /** PARSE **/
  num = 0;
  char* pch = strtok(data, "<");
  while( (pch = strtok(NULL, "<")) != NULL
      && pch - data < DATA_LEN)
  {
    if(pch[0] != 'd' || pch[1] != 't') continue;
    printToOLED(pch+6, BLACK, 0, ROW(15));
    char* end = strtok(NULL, "<");
    pch = strtok(pch, ": ");
    while((pch = strtok(NULL, ": ")) != NULL && pch < end) {
      strncpy(results[num++], pch, MAX_WORD_SIZE);
    }
    break;
  }
  
//  char *pch;
//  for(int i = 0; i < strlen(data) && i < DATA_LEN; i++) {
//    int begin,end;
//    if(data[i++] != '<') continue;
//    if(data[i++] != 'd') continue;
//    if(data[i++] != 't') continue;
//    if(data[i++] != '>') continue;
//    
//    begin = i;
//    while(data[i++] != '<'
//        || data[i] != '/'
//        || data[i+1] != 'd'
//        || data[i+2] != 't'
//        || data[i+3] != '>');
//    end = i;
//    pch = strtok(data+begin, ": ");
//    printToOLED(pch, BLACK, 0, ROW(15));
//    while((pch = strtok(NULL, ": ")) != NULL && pch < data+end) {
//      strncpy(results[num++], pch, MAX_WORD_SIZE);
//    }
//    break;
//  }
  //printToOLED(pch+6, BLACK, 0, ROW(15));

}

int xPos = COL(0);
int yPos = ROW(1);
void reset() {
  oled.setCursor(0, oled.getCursorY()+ROW(1));
}

int writeChar(char c) {
  if(oled.getCursorY() >= ROW(15)) return 1;
  if(oled.getCursorX() > COL(20)) reset();
  if(c == '\n') reset();
  else oled.write(c);
  if(oled.getCursorX() > COL(20)) reset();
  return 0;
}
int wifiAppPrint() {
    /** PRINT RESULTS **/
  n = sprintf(buf, "%s:", word);
  printToOLED(buf, BLACK, COL(0), ROW(0));
  
  reset();
  writeChar(' ');
  for(int i = 0; i < num; i++) {
    xCoord[i] = oled.getCursorX();
    yCoord[i] = oled.getCursorY();
    for(int j = 0; j < strlen(results[i]); j++)
      writeChar(results[i][j]);
    writeChar(' ');
  }
//  n = sprintf(buf, "num=%d", num);
//  printToOLED(buf, BLACK, 0, ROW(row++));

  /** PRINT RAW DATA **/
  //printToOLED(data, BLACK, 0, ROW(15));
}

int wifiColorChoice(uint16 color) {
  oled.setCursor(xCoord[choice], yCoord[choice]);
  oled.setTextColor(color, WHITE);
  for(int j = 0; j < strlen(results[choice]); j++)
    writeChar(results[choice][j]);
  oled.setTextColor(BLACK, WHITE);
}

int wifi_app() {

  while(1) {
    num = 0;
    choice = 0;
    memset(&http_req, '\0', sizeof(http_req));
    memset(&data, '\0', sizeof(data));
    memset(&results, '\0', sizeof(results));
    memset(&xCoord, '\0', sizeof(xCoord));
    memset(&yCoord, '\0', sizeof(yCoord));

    wifiAppGet();
    wifiAppParse();
    wifiAppPrint();
    
    int userInput = 0;
    wifiColorChoice(RED);
    while((userInput = input()) != 0) {
      wifiColorChoice(BLACK);
      choice += userInput;
      choice = choice < 0 ? 0 : choice;
      wifiColorChoice(RED);
      
      n = sprintf(buf, "choice: %d", choice);
      printToOLED(buf, BLACK, COL(0), ROW(15));
    }
    
    strncpy(word, results[choice], MAX_WORD_SIZE);
    oled.fillScreen(WHITE);
  }
  
  return 0;
}

int inputPos = 0;
bool inputSelect = 0;
int input() {
  uint32_t delayVal = 1000;
  static int retVal = -1;

  if(retVal == -1) goto DOWN;
  if(retVal == 1) goto UP;
  while(1) {
    DOWN:
    setColor(R);
    delay(delayVal);
    setColor(R_OFF);
    retVal = -1;
    if(!WyzBeeGpio_Get(4E)) {
      setColor(OFF);
      return retVal;
    }
 
    UP:
    setColor(G);
    delay(delayVal);
    setColor(G_OFF);
    retVal = 1;
    if(!WyzBeeGpio_Get(4E)) {
      setColor(OFF);
      return retVal;
    }

    OK:
    setColor(B);
    delay(delayVal);
    setColor(B_OFF);
    retVal = 0;
    if(!WyzBeeGpio_Get(4E)) {
      setColor(OFF);
      return retVal;
    }
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
