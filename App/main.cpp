#include "main.h"

#include "bt.h"
#include "remote.h"

int n = 0;
char buf[336] = {0};
const char* master_str = "00:23:A7:80:59:ED"; // 71, master
const char* slave_str = "00:23:A7:80:59:F9"; // 990, slave

int main()
{
  bt_init("jmm-slave");
  slave();
}
