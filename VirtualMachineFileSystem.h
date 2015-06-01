#ifndef VIRTUALMACHINEFILESYSTEM_H
#define VIRTUALMACHINEFILESYSTEM_H

#include <stdint.h>

#include <map>
#include <string>

#include "VirtualMachine.h"

class File;
class Directory;
class FatFileSystem;
typedef int TFatBytePtr;
typedef int TFatSectorPtr;
typedef uint16_t TFatClusterPtr;

class FatFileSystem
{
  private:
    int mountFD;

    unsigned int bytesPerSector;
    unsigned int sectorsPerCluster;
    unsigned int numFats;
    unsigned int numRootEntries;

    TFatSectorPtr firstBpbSector;
    unsigned int numBpbSectors;

    TFatSectorPtr firstFatSector;
    unsigned int numFatSectors;

    TFatSectorPtr firstRootSector;
    unsigned int numRootSectors;

    TFatSectorPtr firstDataSector;
    unsigned int numDataSectors;

    unsigned int RootDirSectors;
    unsigned int RootDirClusters;
    unsigned int FirstDataSector;
    unsigned int FirstSectorofCluster1;
    unsigned int FirstSectorofCluster2;

    TFatClusterPtr *FAT;
    uint8_t *rootDir;
    std::string currentDirectory;

    int lastFD;
    std::map<int, File*> files;
    std::map<int, Directory*> directories;

    TFatBytePtr currentByte;

  public:
    FatFileSystem(const char* mount);
    ~FatFileSystem();

    void readBPB();
    void readFAT();
    void readRoot();

    void parseRoot();

    TVMStatus readDirEnt(TFatBytePtr base, SVMDirectoryEntry *dirent);

    TFatClusterPtr getCluster(int byte);
    TFatSectorPtr getSector(int byte);
    TVMStatus read(void* data, int *length);
    TVMStatus write(void* data, int *length);
    TVMStatus seekByte(TFatBytePtr base, TFatBytePtr offset);
    TVMStatus seekSector(TFatSectorPtr base, TFatSectorPtr offset);
    TVMStatus seekCluster(TFatClusterPtr base, TFatClusterPtr offset);

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
    const TFatClusterPtr firstCluster;
    const TFatBytePtr dirPtr;

    TFatBytePtr currentByte;

  public:

    File(int fd, int flags, int mode, TFatBytePtr dirPtr);

    int getFD();

    void read(void *data, int *length);
    void write(void *data, int *length);
    void seek(int offset, int whence, int *newoffset);
};

class Directory
{
  private:
    const int fd;
    const uint16_t firstCluster;
    const TFatBytePtr dirPtr;

    TFatBytePtr currentByte;

  public:

    Directory(int fd, TFatBytePtr dirPtr);

    int getFD();

    void read(void *data, int *length);
    void write(void *data, int *length);
    void rewind();
    void unlink();
};
