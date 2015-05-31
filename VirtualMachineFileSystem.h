#ifndef VIRTUALMACHINEFILESYSTEM_H
#define VIRTUALMACHINEFILESYSTEM_H

#include <stdint.h>

class FatFileSystem
{
  private:
    int mountFD;
    uint16_t BPB_BytsPerSec;
    uint8_t BPB_SecPerClus;

    uint16_t *FAT;
  public:
    FatFileSystem(const char* mount);
    ~FatFileSystem();

    void readBPB();
    bool seekSector(int base, int offset);
    bool seekCluster(int base, int offset);
};
#endif
