
#ifndef LAB2_INT_H
#define LAB2_INT_H

#include <WyzBee.h>

#include <WyzBee_ext.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#define EXT_PORT 					2u
#define NUM_INTERVALS 		256
#define TIMER_LOAD_VAL 		5000000 //5000000 value signifies a time of 1sec (pll clk 80Mhz/prescalar value 80/16=5Mhz (1/5Mhz=t) (reload value= no of sec or ms /t))

enum color_t {R, G, B, ON, R_OFF, G_OFF, B_OFF, OFF};
enum interval_t {NOT_READY=-1, LOW=0, HIGH=1, BREAK=2, LONG=3};
//enum key_t {NONE=-1, KEY_0=0, KEY_1=1, KEY_2=3, KEY_3=4, KEY_4=4, KEY_5=5, KEY_6=6, KEY_7=7, KEY_8=8, KEY_9=9};


extern volatile uint32_t intervals[NUM_INTERVALS];
extern volatile uint32_t lastInterval;
extern volatile uint32_t pos;


extern Adafruit_SSD1351 oled; //@  OLED class variable


int initOled();
int initExtInt();
int initTimer();

void setColor(uint8_t color, uint32_t delay=0);
void extInt();
void timerInt();
interval_t interperetInterval(uint32_t interval);
interval_t nextBit();
uint8_t nextByte();
char readInput();

void reset();

#endif
