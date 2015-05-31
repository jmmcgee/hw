#ifndef VIRTUALMACHINEFILESYSTEM_H
#define VIRTUALMACHINEFILESYSTEM_H

#include <stdint.h>

class FatFileSystem
{
  private:
    int mountFD;
    uint16_t BPB_BytsPerSec;
    uint8_t BPB_SecPerClus;

  public:
    FatFileSystem(const char* mount);
    ~FatFileSystem();

    void readBPB();

};
#endif
