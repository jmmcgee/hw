#ifndef VIRTUALMACHINEFILESYSTEM_H
#define VIRTUALMACHINEFILESYSTEM_H

#include <stdint.h>

#include "VirtualMachine.h"

class FatFileSystem
{
  private:
    int mountFD;

    uint16_t  BPB_BytsPerSec;
    uint8_t   BPB_SecPerClus;
    uint16_t  BPB_RsvdSecCnt;
    uint8_t   BPB_NumFATs;
    uint16_t  BPB_RootEntCnt;
    uint16_t  BPB_FATSz16;

    unsigned int firstBpbSector;
    unsigned int numBpbSectors;

    unsigned int firstFatSector;
    unsigned int numFatSectors;

    unsigned int firstRootSector;
    unsigned int numRootSectors;

    unsigned int firstDataSector;
    unsigned int numDataSectors;

    unsigned int RootDirSectors;
    unsigned int RootDirClusters;
    unsigned int FirstDataSector;
    unsigned int FirstSectorofCluster1;
    unsigned int FirstSectorofCluster2;


    uint16_t *FAT;
    uint8_t *RootDir;

    SVMDirectoryEntry *directory;

  public:
    FatFileSystem(const char* mount);
    ~FatFileSystem();

    void readBPB();
    void readFAT();
    void readRoot();
    void parseRoot() const;

    bool seekSector(int base, int offset);
    bool seekCluster(int base, int offset);
};
#endif
