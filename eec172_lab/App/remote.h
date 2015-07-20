
#ifndef LAB2_INT_H
#define LAB2_INT_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#define EXT_PORT          2u
#define NUM_INTERVALS     256
#define NUM_BYTES         1024
#define TIMER_LOAD_VAL    5000000 //5000000 value signifies a time of 1sec (pll clk 80Mhz/prescalar value 80/16=5Mhz (1/5Mhz=t) (reload value= no of sec or ms /t))
#define INPUT_TIMEOUT_THRESH      10

enum color_t {R, G, B, ON, R_OFF, G_OFF, B_OFF, OFF};
enum interval_t {NOT_READY=-1, LOW=0, HIGH=1, BREAK=2, LONG=3};

extern Adafruit_SSD1351 oled; //@  OLED class variable
extern volatile char lastKey;


int initOled();
int initExtInt();
int initTimer();

void setColor(uint8_t color, uint32_t delay=0);
void extInt();
void timerInt();

interval_t interperetInterval(uint32_t interval);
interval_t nextBit();
int readyByte();
uint8_t nextByte();
char readInput();
int updateIR();
int IRDelay(uint32_t ms);

void reset();

extern volatile char printKey;
extern volatile int inputTimeout; //how long has button been held
#endif
