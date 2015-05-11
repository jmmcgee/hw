#include "Machine.h"
#include "VirtualMachineMemory.h"

MemoryManager* MemoryManager::ref = NULL;

MemoryManager* MemoryManager::get()
{
  if(ref == NULL)
    ref = new MemoryManager();
  return ref; 
}

MemoryManager::MemoryManager()
{
}
