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
  
  uint8_t bpb[512] = {0};
  int len;
  tm->requestFileRead(mountFD, bpb, &len);

  cerr << "SUCCESSFULLY READ " << len << " bytes\n" << flush;
  
  int tempFD;
  tm->requestFileOpen("temp", O_RDWR, 0644, &tempFD);
  tm->requestFileWrite(tempFD, (void*)"FOOO", &len);
}

FatFileSystem::~FatFileSystem()
{
  TVMStatus status;
  status = VMFileClose(mountFD);
}
