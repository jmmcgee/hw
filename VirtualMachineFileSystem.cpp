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
}

