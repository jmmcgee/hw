#ifndef BT_H
#define BT_H

int bt_init(const char* name);
void master(const char* addr_str);
void slave();

void printDevices();
void printRetVals();
void flush(int row = -1);

#endif
