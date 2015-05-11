#ifndef VIRTUALMACHINEMEMORY_H
#define VIRTUALMACHINEMEMORY_H

#include "VirtualMachine.h"
#include <map>

#ifdef __cplusplus
extern "C" {
#endif


class MemoryPool;
class MemoryManager;

class MemoryManager
{
  static MemoryManager* ref;
  std::map<TVMMemoryPoolID, MemoryPool*> pools;
};

class MemoryPool
{
  private:
    TVMMemoryPoolID id;

    void* base;
    TVMMemorySize size;
    std::map<void*, TVMMemorySize> allocations;
};

#ifdef __cplusplus
}
#endif

#endif

