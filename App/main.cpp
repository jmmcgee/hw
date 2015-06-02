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
#include "Adafruit_SSD1351.h"
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


rsi_scanInfo  				remote_dev[RSI_AP_SCANNED_MAX];
HttpRequest  					http_req;
volatile 	uint8 			fflag = 0;
volatile 	uint32_t 		time = 0;

int8     							scan_dev[11][34];
int8     							message[50];
int8     							http_resp[1000];
int8   								*p_ssid_name;
int8								  status, ix, nbr_scan_devs;



err_t 								err;


//Function Prototypes
int printToOLED(char* stringToPrint, int color, int cursorX, int cursorY);

extern Adafruit_SSD1351 oled = Adafruit_SSD1351(); //@  OLED class variable

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

//Insert function to print to OLED display (Lab1).
int printToOLED(char* stringToPrint, int color, int cursorX, int cursorY){
//#define	BLACK           0x0000
//#define	BLUE            0x001F
//#define	RED             0xF800
//#define	GREEN           0x07E0
//#define CYAN            0x07FF
//#define MAGENTA         0xF81F
//#define YELLOW          0xFFE0
//#define WHITE           0xFFFF
	
	//set color of text
	oled.setTextColor(color);
	
	//get string lendth
	int stringLength = strlen(stringToPrint);
	//set the print cursor to the inital location
	oled.setCursor(cursorX,cursorY);
	//print the string to the screen moving the cursor each time
	for(int i=0; i < stringLength; i++){
			oled.setCursor(10*i+cursorX,cursorY); //set the print cursor to the inital location
			oled.write(stringToPrint[i]); //write a char to screen
	}
	
	return 0;
}

//static variables





int main(void)
{
	err_t err;
	sys_ticks_init();																				/*Initialize Systick timer*/
	/*
	WyzBee_exint_config_t WyzBeeExtIntConfig;
	uint8_t ext_port = 2;
	
	//setup an IR interrupt
	WyzBee_PDL_ZERO(WyzBeeExtIntConfig);
	WyzBeeExtIntConfig.abEnable[ext_port] = FALSE;   // INT2
	WyzBeeExtIntConfig.aenLevel[ext_port] = ExIntFallingEdge; //set interrupt to IR falling edge
	WyzBeeExtIntConfig.apfnExintCallback[ext_port] = &Main_ExtIntCallback1;
	WyzBee_Exint_IR_Init(&WyzBeeExtIntConfig);
	WyzBee_Exint_EnableChannel(ext_port);
	*/
	// Initialize the OLED display according to Lab1
	err_t SPIErr = WyzBeeSpi_Init(&config_stc); //initialize spi
	status = WyzBeeWiFi_Init(); 
	//OLED
	oled.begin(); //OLED screen Initialization
	oled.fillScreen(CYAN); //@fills the OLED screen with black pixels
	oled.setTextSize(1); //OLED set text size
	printToOLED("It's alive", RED, 0,0);
	
	char urlbuf[336] = "http://calbeedemo.appspot.com/greetings?msg=hello";
	char statusbuf[336];
	
			memset(scan_dev,'\0',sizeof(scan_dev));
		//status = WyzBeeWiFi_Init();  					                 //@ initializes the RS9113 wifi module
	  if(status == 0) 
		{
					printToOLED("Init works", BLACK, 0,10);
					
					nbr_scan_devs = WyzBeeWiFi_ScanNetworks(remote_dev); //@ this functions scans the AP's in vicinity and returns the number of networks scanned.
					for (ix = 0; ix < nbr_scan_devs; ix++)
					{							                       //@ returns pointer to a string of each SSID scanned in the network.
						memcpy (scan_dev[ix], remote_dev[ix].ssid, strlen ((const int8 *)remote_dev[ix].ssid)); 	   //@ copying the all SSIDs into a local buffer
					}
					
					
					status = WyzBeeWiFi_ConnectAccessPoint((int8 *)"Wireless Bra", (int8 *)"andpanties2"); //join with access point
				
					if (status == 0)
					{
						// connection access works
						printToOLED("ConnAcc works", BLACK, 0, 20);
						
						status = WyzBeeWiFi_HttpGet((int8 *)urlbuf, &http_req, NULL, NULL);
						sprintf(statusbuf, "%d", status);
						printToOLED("Test", BLACK, 0, 30);

						if (status == 0)
						{
							printToOLED("HttpGet works!", BLACK, 0, 50);
						}
										
					}
				}
		return 0;
			}
	
/*
 *********************************************************************************************************
 *                                           END
 *********************************************************************************************************
 */
