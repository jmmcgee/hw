#ifndef BT_H
#define BT_H

int bt_init(const char* name);
void master();
void slave();

void printDevices();
void printRetVals();
void flush();

#endif
