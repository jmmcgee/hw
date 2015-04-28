#include <stdio.h>
#include <string.h>
#include "common/base_types.h"

enum color_t {red, green, blue};

int init();
int tempAndHumidTest();
int accelTest();
int IRTest();
void setColor(color_t color);

char* printLineBreak(char* buf);
char* printRawData(char* buf, uint8_t addr, uint8_t* data, uint16_t len, bool incrementAddress=false);

