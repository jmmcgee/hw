#include "Machine.h"
#include "VirtualMachineMemory.h"

MemoryManager* MemoryManager::ref = NULL;

MemoryManager* MemoryManager::get()
{
  if(ref == NULL)
    ref = new MemoryManager();
  return ref;
}

MemoryManager::MemoryManager():
  mainheap(NULL),
  poolcount(0)
{
}

MemoryManager::~MemoryManager()
{
  if (mainheap) delete[] (char *) mainheap;
}

void MemoryManager::initializeMainPool(TVMMemorySize size)
{
  if (mainheap) return;
  mainheap = (void *) (new char[size]);

  MemoryPool* mainpool = new MemoryPool(VM_MEMORY_POOL_ID_SYSTEM, mainheap, size);

  pools.insert(pair_poolid_poolref(VM_MEMORY_POOL_ID_SYSTEM, mainpool));
}

MemoryPool* MemoryManager::getPool(TVMMemoryPoolID id)
{
  std::map<TVMMemoryPoolID, MemoryPool*>::iterator pools_it;

  pools_it = pools.find(id);

  if (pools_it == pools.end()) return NULL;

  return pools_it->second;
}

MemoryPool::MemoryPool(TVMMemoryPoolID id, void* base, TVMMemorySize size):
  id(id),
  base(base),
  basesize(size)
{
}

TVMStatus MemoryPool::allocate(TVMMemorySize size, void ** memory)
{
  std::map<void*, TVMMemorySize>::iterator alloc_head_it = allocations.begin();
  std::map<void*, TVMMemorySize>::iterator alloc_tail_it;

  void* start;
  void* end;

  if ((alloc_head_it == allocations.end()))
    alloc_tail_it = allocations.end();
  else
    alloc_tail_it = ++allocations.begin();

  while(alloc_tail_it != allocations.end())
  {
    if ((((char*) alloc_tail_it->first) - ((char*) alloc_head_it->first) - (alloc_head_it->second)) >= size)
      break;

    ++alloc_head_it;
    ++alloc_tail_it;
  }

  start = (alloc_head_it != allocations.end()) ? ((char*) alloc_head_it->first) + alloc_head_it->second : ((char*) base);
  end = (alloc_tail_it != allocations.end()) ? (alloc_tail_it->first) : ((char*) base) + basesize;

  int signedsize = size;

  if (signedsize <= (((char*) end) - ((char*) start)))
  {
    allocations.insert(pair_ptr_size(start, size));
    return VM_STATUS_SUCCESS;
  }

  return VM_STATUS_ERROR_INSUFFICIENT_RESOURCES;
}
