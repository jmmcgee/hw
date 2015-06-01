#include "VirtualMachineFileSystem.h"

#include <fcntl.h>
#include <assert.h>

#include <iostream>
#include <fstream>

#include "VirtualMachine.h"
#include "VirtualMachineInternals.h"

using namespace std;

FatFileSystem::FatFileSystem(const char* mount)
  : mountFD(0),
    FAT(nullptr),
    rootDir(nullptr),
    currentDirectory("/"),
    lastFD(2),
    currentByte(0)
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
  TVMStatus status;

  uint8_t bpb[512] = {0};
  int len = 512;
  status = seekByte(0, 0);
  cerr << "(status=" << status << ")" << "SEEKED to " << currentByte << "\n" << flush;

  status = read(bpb, &len);
  cerr << "(status=" << status << ")" << "READ " << len << " bytes\n" << flush;


  uint32_t  BS_jmpBoot     = *(uint32_t*)(bpb + 0);
  //BS_OEMName
  uint16_t BPB_BytsPerSec = *(uint16_t*)(bpb + 11);
  uint8_t BPB_SecPerClus = *(uint8_t*)(bpb + 13);
  uint16_t BPB_RsvdSecCnt = *(uint16_t*)(bpb + 14);
  uint8_t BPB_NumFATs    = *(uint8_t*)(bpb + 16);
  uint16_t BPB_RootEntCnt = *(uint16_t*)(bpb + 17);
  uint16_t  BPB_TotSec16   = *(uint16_t*)(bpb + 19);
  uint8_t   BPB_Media      = *(uint8_t*)(bpb + 21);
  uint16_t BPB_FATSz16    = *(uint16_t*)(bpb + 22);
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


  // Populate Global Information
  bytesPerSector = BPB_BytsPerSec;
  sectorsPerCluster = BPB_SecPerClus;
  numRootEntries = BPB_RootEntCnt;
  numFats = BPB_NumFATs;

  firstBpbSector = 0;
  numBpbSectors = BPB_RsvdSecCnt;

  firstFatSector = firstBpbSector + numBpbSectors;
  numFatSectors = BPB_FATSz16;

  firstRootSector = firstFatSector + numFatSectors * numFats;
  numRootSectors = ((numRootEntries * 32) + (bytesPerSector - 1)) / bytesPerSector;

  firstDataSector = firstRootSector + numRootSectors;
  numDataSectors = BPB_TotSec32 - firstDataSector;

  FirstSectorofCluster1 = ((1 - 2) * BPB_SecPerClus) + firstDataSector;
  FirstSectorofCluster2 = ((2 - 2) * BPB_SecPerClus) + firstDataSector;

  assert(bytesPerSector == 512);
  assert(sectorsPerCluster == 2);
  assert(numRootEntries == 512);
  assert(numFats == 2);
  assert(numRootSectors * bytesPerSector == numRootEntries * 32);

  std::cerr << "BPB_BytsPerSec = " << (unsigned int) BPB_BytsPerSec
    << ", BPB_SecPerClus = " << (unsigned int) BPB_SecPerClus << "\n";
  std::cerr << "BPB_NumFATs = " << (unsigned int) BPB_NumFATs
    << ", BPB_FATSz16 = " << (unsigned int) BPB_FATSz16 << "\n";
  std::cerr << "BPB_RsvdSecCnt = " << (unsigned int) BPB_RsvdSecCnt 
    << ", BPB_RootEntCnt = " << (unsigned int) BPB_RootEntCnt << "\n";
  std::cerr << "FirstSectorofCluster1 = " << (unsigned int) FirstSectorofCluster1
    << ", FirstSectorofCluster2 = " << (unsigned int) FirstSectorofCluster2 << "\n";
  std::cerr << "\n";

  std::cerr << "firstBpbSector = " << (unsigned int) firstBpbSector
    << ", numBpbSectors = " << (unsigned int) numBpbSectors << "\n";
  std::cerr << "firstRootSector = " << (unsigned int) firstRootSector
    << ", numRootSectors = " << (unsigned int) numRootSectors << "\n";
  std::cerr << "firstFatSector = " << (unsigned int) firstFatSector
    << ", numFatSectors = " << (unsigned int) numFatSectors << "\n";
  std::cerr << "firstDataSector = " << (unsigned int) firstDataSector
    << ", numDataSectors = " << (unsigned int) numDataSectors << "\n";
  std::cerr << endl;
}

void FatFileSystem::readFAT()
{
  TVMStatus status;

  int size = numFats * numFatSectors * bytesPerSector;
  FAT = new uint16_t[size / 2];
  status = seekSector(0, firstFatSector);
  status = read(FAT, &size);

  cerr << "FAT dump head... " << "readFAT size: " << size << "\n" << flush;
  for (int e = 0; e < 16; e++) {  // print 16 lines (256 bytes)
    for (int i = 0; i < 8; i++) { // print 8 groups (16 bytes)
      for (int j = 0; j < 2; j++) // print 2 bytes (4 nibbles), space
        printf("%02X", static_cast<int>(((uint8_t *) FAT)[e * 16 + i * 2 + j]));
      cerr << " " << flush;
    }
    cerr << "\n" << flush;
  }
}

void FatFileSystem::readRoot()
{  // NOTE: THIS IS NOT CORRECT, must account for offset of first data sector

  TVMStatus status;

  int size = numRootSectors * bytesPerSector;
  rootDir = new uint8_t[size];
  status = seekSector(0, firstRootSector);
  status = read(rootDir, &size);

  cerr << "Root dump head... " << "readRoot size: " << size << "\n" << flush;
  for (int e = 0; e < 16; e++) {
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 2; j++)
        printf("%02X", static_cast<int>(rootDir[e * 32 + i * 2 + j]));
      cerr << " " << flush;
    }
    cerr << "\n" << flush;
  }
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
  SVMDirectoryEntry *directory = new SVMDirectoryEntry[numRootEntries];

  int dirent_count = 0;
  int long_entry_n;
  uint16_t datetime_buffer;
  uint8_t *rootDirEntPtr;
  SVMDirectoryEntry *dirEntRef;

  for (unsigned i = 0; i < numRootEntries; i++) {
    rootDirEntPtr = rootDir + i * 32;
    dirEntRef = directory + dirent_count;

    if (*((uint16_t *) (rootDirEntPtr + 26)) == 0) {
      // short entry
      long_entry_n = (*(rootDirEntPtr) | 0x40) - 0x40;

      for (int i = 0; i < 5; i++)
        *(dirEntRef->DLongFileName + 0 + i + (long_entry_n - 1) * 13)
          =  *((char *) rootDirEntPtr + 1 + i * 2);
      for (int i = 0; i < 6; i++)
        *(dirEntRef->DLongFileName + 5 + i + (long_entry_n - 1) * 13)
          = *((char *) rootDirEntPtr + 14 + i * 2);
      for (int i = 0; i < 2; i++)
        *(dirEntRef->DLongFileName + 11 + i + (long_entry_n - 1) * 13)
          = *((char *) rootDirEntPtr + 28 + i * 2);

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

      cerr << "directory.DLongFileName = " << dirEntRef->DLongFileName << "\n";
      cerr << "directory.DShortFileName = " << dirEntRef->DShortFileName << "\n";

      cerr << "directory.DAttributes = " << hex << dirEntRef->DAttributes << "\n";
      cerr << "directory.DSize = " << dec << dirEntRef->DSize << "\n";
      cerr << flush;
    }

  }
}

TFatClusterPtr FatFileSystem::getCluster(TFatBytePtr byte)
{
  if( getSector(byte) > firstDataSector )
    return -1;
  // NOTE: THIS IS NOT CORRECT, must account for offset of first data sector
  return byte / (bytesPerSector * sectorsPerCluster);
}

TFatSectorPtr FatFileSystem::getSector(TFatBytePtr byte)
{
  return byte / bytesPerSector;
}

TVMStatus FatFileSystem::read(void* data, int *length)
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  status = tm->requestFileRead(mountFD, data, length);
  currentByte += *length;

  return status;
}

TVMStatus FatFileSystem::write(void* data, int *length)
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  status = tm->requestFileWrite(mountFD, data, length);
  currentByte += *length;

  return status;
}

TVMStatus FatFileSystem::seekByte(TFatBytePtr base, TFatBytePtr offset)
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  status = tm->requestFileSeek(mountFD, offset, base, &currentByte);

  if (currentByte == offset + base)
    return VM_STATUS_SUCCESS;
  else
    return VM_STATUS_FAILURE;
}

TVMStatus FatFileSystem::seekSector(TFatSectorPtr base, TFatSectorPtr offset)
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  offset *= bytesPerSector;
  base *= bytesPerSector;
  status = tm->requestFileSeek(mountFD, offset, base, &currentByte);

  if (currentByte == offset + base)
    return VM_STATUS_SUCCESS;
  else
    return VM_STATUS_FAILURE;
}

TVMStatus FatFileSystem::seekCluster(TFatClusterPtr base, TFatClusterPtr offset)
{
  ThreadManager* tm = ThreadManager::get();
  TVMStatus status;

  offset *= bytesPerSector * sectorsPerCluster;
  base *= bytesPerSector * sectorsPerCluster;
  // NOTE: THIS IS NOT CORRECT, must account for offset of first data sector
  status = tm->requestFileSeek(mountFD, offset, base, &currentByte);

  if (currentByte == offset + base)
    return VM_STATUS_SUCCESS;
  else
    return VM_STATUS_FAILURE;
}

