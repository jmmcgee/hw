
#ifndef LAB2_INT_H
#define LAB2_INT_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#define EXT_PORT           2u
#define NUM_INTERVALS     256
#define NUM_BYTES         1024
#define TIMER_LOAD_VAL     5000000 //5000000 value signifies a time of 1sec (pll clk 80Mhz/prescalar value 80/16=5Mhz (1/5Mhz=t) (reload value= no of sec or ms /t))

enum color_t {R, G, B, ON, R_OFF, G_OFF, B_OFF, OFF};

extern Adafruit_SSD1351 oled; //@  OLED class variable


int initOled();

void setColor(uint8_t color, uint32_t delay=0);

#endif
