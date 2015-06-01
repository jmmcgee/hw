#include "VirtualMachineFileSystem.h"

#include <fcntl.h>

#include <iostream>
#include <fstream>

#include "VirtualMachine.h"
#include "VirtualMachineInternals.h"

using namespace std;

FatFileSystem::FatFileSystem(const char* mount)
  : mountFD(0)
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  status = tm->requestFileOpen(mount, O_RDWR, 0644, &mountFD);
  cerr << "OPENED MOUNT: status=" << status << endl;

  readBPB();
  readFAT();
  readRoot();
  parseRoot();
}

FatFileSystem::~FatFileSystem()
{
  TVMStatus status;
  status = VMFileClose(mountFD);
}

void FatFileSystem::readBPB()
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  uint8_t bpb[512] = {0};
  int len = 512;
  int offset = 0;
  status = tm->requestFileSeek(mountFD, 0, 0, &offset);
  cerr << "(status=" << status << ")" << "SEEKED to " << offset << "\n" << flush;

  status = tm->requestFileRead(mountFD, bpb, &len);
  cerr << "(status=" << status << ")" << "READ " << len << " bytes\n" << flush;


  uint32_t  BS_jmpBoot     = *(uint32_t*)(bpb + 0);
  //BS_OEMName
  BPB_BytsPerSec = *(uint16_t*)(bpb + 11);
  BPB_SecPerClus = *(uint8_t*)(bpb + 13);
  BPB_RsvdSecCnt = *(uint16_t*)(bpb + 14);
  BPB_NumFATs    = *(uint8_t*)(bpb + 16);
  BPB_RootEntCnt = *(uint16_t*)(bpb + 17);
  uint16_t  BPB_TotSec16   = *(uint16_t*)(bpb + 19);
  uint8_t   BPB_Media      = *(uint8_t*)(bpb + 21);
  BPB_FATSz16    = *(uint16_t*)(bpb + 22);
  uint16_t  BPB_SecPerTrk  = *(uint16_t*)(bpb + 24);
  uint16_t  BPB_NumHeads   = *(uint16_t*)(bpb + 26);
  uint32_t  BPB_HiddSec    = *(uint32_t*)(bpb + 28);
  uint32_t  BPB_TotSec32   = *(uint32_t*)(bpb + 32);
  uint8_t   BS_DrvNum      = *(uint8_t*)(bpb + 36);
  uint8_t   BS_Reserved1   = *(uint8_t*)(bpb + 37);
  uint8_t   BS_BootSig     = *(uint8_t*)(bpb + 38);
  uint32_t  BS_VolID       = *(uint32_t*)(bpb + 39);
  //BS_VolLab
  //uint_t BS_FilSysType

  firstBpbSector = 0;
  numBpbSectors = BPB_RsvdSecCnt;

  firstFatSector = firstBpbSector + numBpbSectors;
  numFatSectors = BPB_NumFATs * BPB_FATSz16;

  firstRootSector = firstFatSector + numFatSectors;
  numRootSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec - 1)) / BPB_BytsPerSec;

  firstDataSector = firstRootSector + numRootSectors;
  numDataSectors = BPB_TotSec32 - firstDataSector;

  FirstSectorofCluster1 = ((1 - 2) * BPB_SecPerClus) + firstDataSector;
  FirstSectorofCluster2 = ((2 - 2) * BPB_SecPerClus) + firstDataSector;

  std::cerr << "BPB_BytsPerSec = " << (unsigned int) BPB_BytsPerSec << ", BPB_SecPerClus = " << (unsigned int) BPB_SecPerClus << "\n" << std::flush;
  std::cerr << "BPB_NumFATs = " << (unsigned int) BPB_NumFATs << ", BPB_FATSz16 = " << (unsigned int) BPB_FATSz16 << "\n" << std::flush;
  std::cerr << "BPB_RsvdSecCnt = " << (unsigned int) BPB_RsvdSecCnt << ", BPB_RootEntCnt = " << (unsigned int) BPB_RootEntCnt << "\n" << std::flush;
  std::cerr << "FirstSectorofCluster1 = " << (unsigned int) FirstSectorofCluster1 << ", FirstSectorofCluster2 = " << (unsigned int) FirstSectorofCluster2 << "\n" << std::flush;

  std::cerr << "firstBpbSector = " << (unsigned int) firstBpbSector << ", numBpbSectors = " << (unsigned int) numBpbSectors << "\n" << std::flush;
  std::cerr << "firstFatSector = " << (unsigned int) firstFatSector << ", numFatSectors = " << (unsigned int) numFatSectors << "\n" << std::flush;
  std::cerr << "firstRootSector = " << (unsigned int) firstRootSector << ", numRootSectors = " << (unsigned int) numRootSectors << "\n" << std::flush;
  std::cerr << "firstDataSector = " << (unsigned int) firstDataSector << ", numDataSectors = " << (unsigned int) numDataSectors << "\n" << std::flush;
}

void FatFileSystem::readFAT()
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  int size = BPB_NumFATs * BPB_FATSz16 * BPB_BytsPerSec;

  FAT = new uint16_t[size];
  status = tm->requestFileRead(mountFD, FAT, &size);

  cerr << "FAT dump head... " << "readFAT size: " << size << "\n" << flush;
  for (int e = 0; e < 16; e++) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 2; j++)
        printf("%02X", static_cast<int>(((uint8_t *) FAT)[e * 16 + i * 2 + j]));
      cerr << " " << flush;
    }
    cerr << "\n" << flush;
  }
}

void FatFileSystem::readRoot()
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  int size = numRootSectors * BPB_BytsPerSec;

  RootDir = new uint8_t[size];
  status = tm->requestFileRead(mountFD, RootDir, &size);

  cerr << "Root dump head... " << "readRoot size: " << size << "\n" << flush;
  for (int e = 0; e < 16; e++) {
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 2; j++)
        printf("%02X", static_cast<int>(RootDir[e * 32 + i * 2 + j]));
      cerr << " " << flush;
    }
    cerr << "\n" << flush;
  }

  // RootDirClusters = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec - 1)) / (BPB_BytsPerSec * BPB_SecPerClus);
  // for(int i = 0; i < RootDirClusters; i++) {
  //   int len = BPB_BytsPerSec * BPB_SecPerClus;
  //   uint8_t *data = new uint8_t[len];
  //   status = tm->requestFileRead(mountFD, data, &len);
  //
  //   for(int j = 0; j < len; j++) {
  //     cerr << hex << data[j] << flush;
  //   }
  // }

  /*
  for (int i = 0; i < BPB_RootEntCnt; i++) {
    readDir();
  }
  */
}
//
// typedef struct{
//     unsigned int DYear;
//     unsigned char DMonth;
//     unsigned char DDay;
//     unsigned char DHour;
//     unsigned char DMinute;
//     unsigned char DSecond;
//     unsigned char DHundredth;
// } SVMDateTime, *SVMDateTimeRef;
//
// typedef struct{
//     char DLongFileName[VM_FILE_SYSTEM_MAX_PATH];
//     char DShortFileName[VM_FILE_SYSTEM_SFN_SIZE];
//     unsigned int DSize;
//     unsigned char DAttributes;
//     SVMDateTime DCreate;
//     SVMDateTime DAccess;
//     SVMDateTime DModify;
// } SVMDirectoryEntry, *SVMDirectoryEntryRef;

void FatFileSystem::parseRoot() const
{
  SVMDirectoryEntry *directory = new SVMDirectoryEntry[BPB_RootEntCnt];

  int dirent_count = 0;
  int long_entry_n;
  uint16_t datetime_buffer;
  uint8_t *rootDirEntPtr;
  SVMDirectoryEntry *dirEntRef;

  for (int i = 0; i < BPB_RootEntCnt; i++) {
    rootDirEntPtr = RootDir + i * 32;
    dirEntRef = directory + dirent_count;

    if (*((uint16_t *) (rootDirEntPtr + 26)) == 0) {
      // short entry
      long_entry_n = (*(rootDirEntPtr) | 0x40) - 0x40;

      for (int i = 0; i < 5; i++)
        *(dirEntRef->DLongFileName + 0 + i + (long_entry_n - 1) * 13) =  *((char *) rootDirEntPtr + 1 + i * 2);
      for (int i = 0; i < 6; i++)
        *(dirEntRef->DLongFileName + 5 + i + (long_entry_n - 1) * 13) = *((char *) rootDirEntPtr + 14 + i * 2);
      for (int i = 0; i < 2; i++)
        *(dirEntRef->DLongFileName + 11 + i + (long_entry_n - 1) * 13) = *((char *) rootDirEntPtr + 28 + i * 2);

      if (*(rootDirEntPtr) & 0x40)
      *(dirEntRef->DLongFileName + 13 + (long_entry_n - 1) * 13) = '\0';
    } else {
      // long entry
      memcpy(&(dirEntRef->DShortFileName), rootDirEntPtr + 0, 11);
      *(dirEntRef->DShortFileName + 11) = '\0';
      memcpy(&(dirEntRef->DAttributes), rootDirEntPtr + 12, 1);
      memcpy(&(dirEntRef->DSize), rootDirEntPtr + 28, 4);

      memcpy(&datetime_buffer, rootDirEntPtr + 14, 2);
      cerr << "created time = " << hex << datetime_buffer << "\n" << flush;


      memcpy(&datetime_buffer, rootDirEntPtr + 16, 2);
      cerr << "created date = " << hex << datetime_buffer << "\n" << flush;


      memcpy(&datetime_buffer, rootDirEntPtr + 18, 2);
      cerr << "access date = " << hex << datetime_buffer << "\n" << flush;


      memcpy(&datetime_buffer, rootDirEntPtr + 22, 2);
      cerr << "modified time = " << hex << datetime_buffer << "\n" << flush;


      memcpy(&datetime_buffer, rootDirEntPtr + 24, 2);
      cerr << "modified date = " << hex << datetime_buffer << "\n" << flush;


      ++dirent_count;

      // cerr << "directory.DLongFileName = " << dirEntRef->DLongFileName << "\n" << flush;
      // cerr << "directory.DShortFileName = " << dirEntRef->DShortFileName << "\n" << flush;
      // cerr << "directory.DAttributes = " << hex << dirEntRef->DAttributes << "\n" << flush;
      // cerr << "directory.DSize = " << dec << dirEntRef->DSize << "\n" << flush;
    }

  }
}

bool FatFileSystem::seekSector(int base, int offset)
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  int newoffset;

  offset *= BPB_BytsPerSec;
  base *= BPB_BytsPerSec;

  cerr << "seekSector called with offset * BPB_BytsPerSec = " << offset << ", base * BPB_BytsPerSec = " << base << "\n" << flush;

  status = tm->requestFileSeek(mountFD, offset, base, &newoffset);

  cerr << "seekSector's requestFileSeek status is " << status << "\n" << flush;

  if (newoffset == offset + base)
    return true;
  else
    return false;
}

bool FatFileSystem::seekCluster(int base, int offset)
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  int newoffset;

  offset *= BPB_BytsPerSec * BPB_SecPerClus;
  base *= BPB_BytsPerSec * BPB_SecPerClus;
  status = tm->requestFileSeek(mountFD, offset, base, &newoffset);

  if (newoffset == offset + base)
    return true;
  else
    return false;
}
