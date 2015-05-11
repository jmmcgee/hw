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

TVMMemoryPoolID MemoryManager::createPool(void* base, TVMMemorySize size)
{
  TVMMemoryPoolID poolid = ++poolcount;

  MemoryPool* pool = new MemoryPool(poolid, base, size);

  pools.insert(pair_poolid_poolref(poolid, pool));

  return poolid;
}

TVMMemoryPoolID MemoryManager::deletePool(TVMMemoryPoolID id)
{
  MemoryPool* pool = getPool(id);

  if (!pool) return VM_STATUS_ERROR_INVALID_PARAMETER;

  if (!pool->canDelete()) return VM_STATUS_ERROR_INVALID_STATE;

  pools.erase(id);
  delete pool;
  return VM_STATUS_SUCCESS;
}

MemoryPool::MemoryPool(TVMMemoryPoolID id, void* base, TVMMemorySize size):
  id(id),
  base(base),
  basesize(size)
{
}

MemoryPool::~MemoryPool()
{
}

TVMMemoryPoolID MemoryPool::getId()
{
  return id;
}

TVMMemorySize MemoryPool::getSize()
{
  return basesize;
}

TVMMemorySize MemoryPool::largest()
{
  std::map<void*, TVMMemorySize>::iterator alloc_head_it = allocations.begin();
  std::map<void*, TVMMemorySize>::iterator alloc_tail_it;

  void* start;
  void* end;
  TVMMemorySize empty, largest = 0;

  if ((alloc_head_it == allocations.end()))
    alloc_tail_it = allocations.end();
  else
    alloc_tail_it = ++allocations.begin();

  while(alloc_tail_it != allocations.end())
  {
    if ((empty = (((char*) alloc_tail_it->first) - ((char*) alloc_head_it->first) - (alloc_head_it->second))) > largest)
      largest = empty;

    ++alloc_head_it;
    ++alloc_tail_it;
  }

  start = (alloc_head_it != allocations.end()) ? ((char*) alloc_head_it->first) + alloc_head_it->second : ((char*) base);
  end = (alloc_tail_it != allocations.end()) ? (alloc_tail_it->first) : ((char*) base) + basesize;

  if ((empty = (((char*) end) - ((char*) start))) > largest)
    largest = empty;

  return largest;
}

TVMMemorySize MemoryPool::query()
{
  std::map<void*, TVMMemorySize>::iterator alloc_it = allocations.begin();

  TVMMemorySize reservedbytes = 0;

  while(alloc_it != allocations.end())
  reservedbytes += alloc_it->second;

  return basesize - reservedbytes;
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
    *memory = start;
    return VM_STATUS_SUCCESS;
  }

  return VM_STATUS_ERROR_INSUFFICIENT_RESOURCES;
}

TVMStatus MemoryPool::deallocate(void* memory)
{
  std::map<void*, TVMMemorySize>::iterator alloc_it;

  alloc_it = allocations.find(memory);

  if (alloc_it == allocations.end()) return VM_STATUS_ERROR_INVALID_PARAMETER;

  allocations.erase(alloc_it);

  return VM_STATUS_SUCCESS;
}

bool MemoryPool::canDelete()
{
  return allocations.empty();
}
