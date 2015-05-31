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
  status = VMFileOpen(mount, O_RDWR, 0644, &mountFD);

  cerr << "OPENED MOUNT: status=" << status << endl;
  
  uint8_t bpb[512] = {0};
  int len = 512;
  //status = VMFileRead(mountFD, bpb, &len);
  status = tm->requestFileRead(mountFD, bpb, &len);
  cerr << "READ BPB: status=" << status << endl;

  cerr << "(" << status << ")" << "SUCCESSFULLY READ " << len << " bytes\n" << flush;
  for(int i =0; i < 512; i++)
    cerr << char(bpb[512]);
  
  int tempFD;
  //tm->requestFileOpen("temp", O_RDWR, 0644, &tempFD);
  //tm->requestFileWrite(tempFD, bpb, &len);
  //tm->requestFileClose(tempFD);
}

FatFileSystem::~FatFileSystem()
{
  TVMStatus status;
  status = VMFileClose(mountFD);
}
