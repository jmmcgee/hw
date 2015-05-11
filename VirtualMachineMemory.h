#ifndef VIRTUALMACHINEMEMORY_H
#define VIRTUALMACHINEMEMORY_H

#include "VirtualMachine.h"
#include <map>

#ifdef __cplusplus
extern "C" {
#endif


class MemoryPool;
class MemoryManager;

typedef std::pair<TVMMemoryPoolID, MemoryPool*> pair_poolid_poolref;
typedef std::pair<void*, TVMMemorySize> pair_ptr_size;

class MemoryManager
{
  private:
    static MemoryManager* ref;

    void* mainheap;

    std::map<TVMMemoryPoolID, MemoryPool*> pools;
    size_t poolcount;

  public:
    static MemoryManager* get();

    MemoryManager();
    ~MemoryManager();

    void initializeMainPool(TVMMemorySize size);

    MemoryPool* getPool(TVMMemoryPoolID id);
    TVMMemoryPoolID createPool(void* base, TVMMemorySize size);
    TVMStatus deletePool(TVMMemoryPoolID id);
};

class MemoryPool
{
  private:
    TVMMemoryPoolID id;

    void* base;
    TVMMemorySize basesize;
    std::map<void*, TVMMemorySize> allocations;

  public:
    MemoryPool(TVMMemoryPoolID id, void* base, TVMMemorySize size);
    ~MemoryPool();

    TVMMemoryPoolID getId();
    TVMMemorySize getSize();

    TVMMemorySize largest();
    TVMMemorySize query();
    TVMStatus allocate(TVMMemorySize size, void ** memory);
    TVMStatus deallocate(void* memory);
    bool canDelete();
};

#ifdef __cplusplus
}
#endif

#endif
