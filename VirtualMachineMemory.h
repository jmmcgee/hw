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
    static MemoryManager* get();

    MemoryManager();
    ~MemoryManager();

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
    MemoryPool(void* base, TVMMemorySize size);
    ~MemoryPool();

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

