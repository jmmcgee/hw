#include "VirtualMachineFileSystem.h"

#include <fcntl.h>

#include "VirtualMachine.h"

FatFileSystem::FatFileSystem(const char* mount)
  : mountFD(0)
{
  TVMStatus status;
  status = VMFileOpen(mount, O_TRUNC | O_RDWR, 0644, &mountFD);
}

FatFileSystem::~FatFileSystem()
{
  TVMStatus status;
  status = VMFileClose(mountFD);
}
