#ifndef VIRTUALMACHINEFILESYSTEM_H
#define VIRTUALMACHINEFILESYSTEM_H

#include <stdint.h>

#include <map>

#include "VirtualMachine.h"

class FatFileSystem
{
  private:
    int mountFD;

    unsigned int bytesPerSector;
    unsigned int sectorsPerCluster;
    unsigned int numFats;
    unsigned int numRootEntries;

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
    uint8_t *rootDir;

    uint16_t currentCluster;
    uint16_t currentSector;
    uint16_t currentByte;

    SVMDirectoryEntry *directory;

  public:
    FatFileSystem(const char* mount);
    ~FatFileSystem();

    void readBPB();
    void readFAT();
    void readRoot();
    void parseRoot() const;

    TVMStatus seekByte(int base, int offset);
    TVMStatus seekSector(int base, int offset);
    TVMStatus seekCluster(int base, int offset);

    TVMStatus fileOpen(const char* filename, int flags, int mode, int *fd);
    TVMStatus fileClose(int fd);
    TVMStatus fileRead(int fd, void *data, int *length);
    TVMStatus fileWrite(int fd, void *data, int *length);
    TVMStatus fileSeek(int fd, int offset, int whence, int *newoffset);
    TVMStatus directoryOpen(const char* dirname, int *dirdescriptor);
    TVMStatus directoryClose(int dirdescriptor);
    TVMStatus directoryRead(int dirdescriptor, SVMDirectoryEntryRef dirent);
    TVMStatus directoryRewind(int dirdescriptor);
    TVMStatus directoryCurrent(char *abspath);
    TVMStatus directoryChange(const char* path);
    TVMStatus directoryCreate(const char* dirname); // Extra Credit
    TVMStatus directoryUnlink(const char* path); // Extra Credit

};
#endif

class File
{
  private:
    const int fd;
    const int flags;
    const int mode;
    const uint16_t firstCluster;

    uint16_t currentCluster;
    uint16_t currentSector;
    uint16_t currentByte;

  public:

    File(int fd, int flags, int mode, SVMDirectoryEntry *dirent);

    int getFD();

    read(void *data, int *length);
    write(void *data, int *length);
    seek(int offset, int whence, int *newoffset);
};

class Directory
{
  private:
    const int fd;
    const uint16_t firstCluster;

    uint16_t currentCluster;
    uint16_t currentSector;
    uint16_t currentByte;

  public:

    Directory(int fd, SVMDirectoryEntry *dirent);

    int getFD();

    read(void *data, int *length);
    write(void *data, int *length);
    rewind();
    unlink();
};

