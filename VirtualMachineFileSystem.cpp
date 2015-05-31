#include "VirtualMachineFileSystem.h"

#include <fcntl.h>

#include <iostream>

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
  for(int i =0; i < 512; i++)
    cerr << bpb[i] << "\n" << flush;


    uint32_t BS_jmpBoot = *(uint32_t*)(bpb + 0);
    //BS_OEMName
    BPB_BytsPerSec = *(uint16_t*)(bpb + 11);
    BPB_SecPerClus = *(uint8_t*)(bpb + 13);
    uint16_t BPB_RsvdSecCnt = *(uint16_t*)(bpb + 14);
    uint8_t BPB_NumFATs = *(uint8_t*)(bpb + 16);
    uint16_t BPB_RootEntCnt = *(uint16_t*)(bpb + 17);
    uint16_t BPB_TotSec16 = *(uint16_t*)(bpb + 19);
    uint8_t BPB_Media = *(uint8_t*)(bpb + 21);
    uint16_t BPB_FATSz16 = *(uint16_t*)(bpb + 22);
    uint16_t BPB_SecPerTrk = *(uint16_t*)(bpb + 24);
    uint16_t BPB_NumHeads = *(uint16_t*)(bpb + 26);
    uint32_t BPB_HiddSec = *(uint32_t*)(bpb + 28);
    uint32_t BPB_TotSec32 = *(uint32_t*)(bpb + 32);
    uint8_t BS_DrvNum = *(uint8_t*)(bpb + 36);
    uint8_t BS_Reserved1 = *(uint8_t*)(bpb + 37);
    uint8_t BS_BootSig = *(uint8_t*)(bpb + 38);
    uint32_t BS_VolID = *(uint32_t*)(bpb + 39);
    //BS_VolLab
    //uint_t BS_FilSysType

    std::cerr << "BPB_BytsPerSec = " << BPB_BytsPerSec << ", BPB_SecPerClus = " << (int) BPB_SecPerClus << "\n" << std::flush;
}
