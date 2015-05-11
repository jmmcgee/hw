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
  private:
    static MemoryManager* ref;

    std::map<TVMMemoryPoolID, MemoryPool*> pools;

  public:
    MemoryPool* getPool(TVMMemoryPoolID id);
    TVMMemoryPoolID createPool(void* base, TVMMemorySize size);
    TVMStatus deletePool(TVMMemoryPoolID id);
};

class MemoryPool
{
  private:
    TVMMemoryPoolID id;

    void* base;
    TVMMemorySize size;
    std::map<void*, TVMMemorySize> allocations;

  public:
    TVMMemoryPoolID getId();
    TVMMemorySize getSize();

    TVMMemorySize largest();
    TVMMemorySize query();
    TVMStatus allocate(TVMMemorySize size, void ** memory);
    TVMStatus deallocate(void* memory);
};

#ifdef __cplusplus
}
#endif

#endif

