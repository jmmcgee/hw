#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <deque>
#include <queue>
#include <map>

#include "Machine.h"
#include "VirtualMachine.h"
#include "VirtualMachineMemory.h"


extern "C" TVMMainEntry VMLoadModule(const char *module);
void *sharedmemory;
int sharedsize;


void status(const char* msg = "");
/** Forward Delcarations **/

#define VM_THREAD_PRIORITY_IDLE                  ((TVMThreadPriority)0x00)

typedef struct {
  TVMThreadEntry entry;
  void* param;
  TVMThreadID threadid;
} skeletonCall, *skeletonCallRef;

class ThreadControlBlock {
  public:
    bool ismainthread;

    SMachineContext mcnxt;
    bool signalsOn;

    TVMThreadEntry entry;
    void* param;
    skeletonCallRef call;

    char *stackaddr;
    TVMMemorySize stacksize;

    volatile TVMTick sleepcounter;
    volatile TVMThreadID id;
    volatile TVMThreadPriority prio;
    volatile TVMThreadState state;

    ThreadControlBlock(bool ismainthread);
    ThreadControlBlock(TVMThreadEntry entry, void* param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadID id);
    ~ThreadControlBlock();

    TVMThreadID getId();
    TVMThreadState getState();
    TVMThreadPriority getPrio();
    SMachineContextRef getMcnxtRef();
    TMachineSignalState disable();
    void enableSignals();
    void disableSignals();
    void restoreSignals();

    void activate();
    void terminate();
    void dead();
    void sleep(TVMTick ticks);
    void ready();
    void running();
    void waiting();
    TVMTick getSleepcounter();
    void updateSleepcounter();

    static void skeletonEntry(void* call);
};

class ThreadManager {
  public:
    ThreadControlBlock *currentthread;
    unsigned int threadcounter;

    std::deque<ThreadControlBlock*> threadqueue_ready_low;
    std::deque<ThreadControlBlock*> threadqueue_ready_med;
    std::deque<ThreadControlBlock*> threadqueue_ready_high;

    std::deque<ThreadControlBlock*> threadqueue_sleeping;
    std::deque<ThreadControlBlock*> threadqueue_waiting;
    std::deque<ThreadControlBlock*> threadqueue_dead;

    ThreadManager();
    ~ThreadManager();

    TVMThreadID getNewId();

    ThreadControlBlock* getCurrentThread();

    ThreadControlBlock* findThread(TVMThreadID id);

    TVMStatus activateThread(TVMThreadID id);
    TVMStatus terminateThread(TVMThreadID id);

    void replaceThread();
    void popFromAll(ThreadControlBlock* thread);
    void popFromWaiting(ThreadControlBlock* thread);
    void pushToDead(ThreadControlBlock *thread);
    void pushToSleep(ThreadControlBlock* thread, TVMTick tick);
    void pushToReady(ThreadControlBlock* thread);
    void pushToWaiting(ThreadControlBlock* thread);

    void updateSleepingThreads();

    ThreadControlBlock *idlethread;
    static void idleloop(void* param);

    static void requestFileOperationCallback(void* calldata, int result);
    TVMStatus requestFileOpen(const char *filename, int flags, int mode, int *filedescriptor);
    TVMStatus requestFileWrite(int filedescriptor, void *data, int *length);
    TVMStatus requestFileSeek(int filedescriptor, int offset, int whence, int *newoffset);
    TVMStatus requestFileRead(int filedescriptor, void *data, int *length);
    TVMStatus requestFileClose(int filedescriptor);
};

class SignalGuard
{
  public:
    ThreadControlBlock* thread;
    bool released;

    SignalGuard(ThreadControlBlock* t);
    ~SignalGuard();

    void lock();
    void unlock();
};

class MutexManager
{
  public:
    TVMMutexID lastID;
    std::map<TVMMutexID, std::deque<TVMThreadID>* > mutexqueues;

    MutexManager();
    ~MutexManager();
};


void MachineAlarmCallback(void *calldata);


/** VM Globals **/


MemoryManager *memorymanager;
ThreadManager *threadmanager;
MutexManager *mutexmanager;
char buf[1024];


/** VM Thread API **/

extern TVMMainEntry VMLoadModule(const char *module);
TVMStatus VMStart(int tickms, TVMMemorySize heapsize, int machinetickms,
    TVMMemorySize ssize, const char *mount, int argc, char *argv[])
{
  sharedmemory = MachineInitialize(machinetickms, ssize);;
  sharedsize = ssize;

  memorymanager = MemoryManager::get();
  memorymanager->initializeMainPool(heapsize);
  threadmanager = new ThreadManager;
  mutexmanager = new MutexManager;

  MachineEnableSignals();
  MachineRequestAlarm(tickms * 1000, MachineAlarmCallback, NULL);

  TVMMainEntry vmmain = VMLoadModule(argv[0]);
  if (!vmmain) return VM_STATUS_FAILURE;

  vmmain(argc,argv);

  MachineTerminate();

  return VM_STATUS_SUCCESS;
}

TVMStatus VMThreadCreate(TVMThreadEntry entry, void *param,
    TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadIDRef tid)
{
  if (!entry || !tid) return VM_STATUS_ERROR_INVALID_PARAMETER;

  TMachineSignalState sigstate;
  MachineSuspendSignals(&sigstate);

  *tid = threadmanager->getNewId();
  ThreadControlBlock *tcb_ptr = new ThreadControlBlock(entry, param, memsize, prio, *tid);

  threadmanager->pushToDead(tcb_ptr);

  MachineResumeSignals(&sigstate);

  return VM_STATUS_SUCCESS;
}

TVMStatus VMThreadDelete(TVMThreadID thread)
{
  ThreadControlBlock *tcb_ptr = threadmanager->findThread(thread);

  if (!tcb_ptr) return VM_STATUS_ERROR_INVALID_ID;

  if (tcb_ptr->getState() != VM_THREAD_STATE_DEAD) return VM_STATUS_ERROR_INVALID_STATE;

  TMachineSignalState sigstate;
  MachineSuspendSignals(&sigstate);

  threadmanager->popFromAll(tcb_ptr);

  if (threadmanager->getCurrentThread() == tcb_ptr) {
    MachineResumeSignals(&sigstate);
    threadmanager->replaceThread();
  }

  delete tcb_ptr;

  MachineResumeSignals(&sigstate);

  return VM_STATUS_SUCCESS;
}

TVMStatus VMThreadActivate(TVMThreadID thread)
{
  TMachineSignalState sigstate;
  MachineSuspendSignals(&sigstate);

  return threadmanager->activateThread(thread);

  MachineResumeSignals(&sigstate);
}

TVMStatus VMThreadTerminate(TVMThreadID thread)
{
  TMachineSignalState sigstate;
  MachineSuspendSignals(&sigstate);

  TVMStatus terminatereturn = threadmanager->terminateThread(thread);

  MachineResumeSignals(&sigstate);

  threadmanager->replaceThread();

  return terminatereturn;
}

TVMStatus VMThreadID(TVMThreadIDRef threadref)
{
  if (!threadref) return VM_STATUS_ERROR_INVALID_PARAMETER;

  *threadref = threadmanager->getCurrentThread()->getId();

  return VM_STATUS_SUCCESS;
}

TVMStatus VMThreadState(TVMThreadID thread, TVMThreadStateRef stateref)
{
  if (!stateref) return VM_STATUS_ERROR_INVALID_PARAMETER;

  ThreadControlBlock *tcb_ptr = threadmanager->findThread(thread);

  if (!tcb_ptr) return VM_STATUS_ERROR_INVALID_ID;

  *stateref = tcb_ptr->getState();

  return VM_STATUS_SUCCESS;
}

TVMStatus VMThreadSleep(TVMTick tick)
{
  if (tick == VM_TIMEOUT_INFINITE) return VM_STATUS_ERROR_INVALID_PARAMETER;

  TMachineSignalState sigstate;
  MachineSuspendSignals(&sigstate);

  threadmanager->pushToSleep(threadmanager->getCurrentThread(), tick);

  MachineResumeSignals(&sigstate);

  if (tick == VM_TIMEOUT_IMMEDIATE) threadmanager->replaceThread();

  while(threadmanager->getCurrentThread()->getSleepcounter());

  return VM_STATUS_SUCCESS;
}


/** VM Mutex API **/


TVMStatus VMMutexCreate(TVMMutexIDRef mutexref)
{
  SignalGuard signalGuard(threadmanager->currentthread);
  signalGuard.lock();

  TVMMutexID mutex = mutexmanager->lastID+1;

  while(mutexmanager->mutexqueues.find(mutex) != mutexmanager->mutexqueues.end()) {
    mutex++;
    if(mutex == mutexmanager->lastID)
      return VM_STATUS_ERROR_INSUFFICIENT_RESOURCES;
  }

  *mutexref = mutex;

  mutexmanager->mutexqueues[mutex] = new std::deque<TVMThreadID>;

  return VM_STATUS_SUCCESS;
}

TVMStatus VMMutexDelete(TVMMutexID mutex)
{
  SignalGuard signalGuard(threadmanager->currentthread);
  signalGuard.lock();

  std::map<TVMMutexID, std::deque<TVMThreadID>* >::iterator mutexqueues_it =  mutexmanager->mutexqueues.find(mutex);
  std::deque<TVMThreadID>* q;

  if(mutexqueues_it == mutexmanager->mutexqueues.end())
    return VM_STATUS_ERROR_INVALID_ID;
  q = mutexqueues_it->second;

  if(q == NULL ||
      !q->empty()
    )
    return VM_STATUS_ERROR_INVALID_STATE;

  delete q;
  mutexmanager->mutexqueues.erase(mutexqueues_it);

  return VM_STATUS_SUCCESS;
}

TVMStatus VMMutexQuery(TVMMutexID mutex, TVMThreadIDRef ownerref)
{
  SignalGuard signalGuard(threadmanager->currentthread);
  signalGuard.lock();

  std::map<TVMMutexID, std::deque<TVMThreadID>* >::iterator mutexqueues_it =  mutexmanager->mutexqueues.find(mutex);
  std::deque<TVMThreadID>* q;

  if(mutexqueues_it == mutexmanager->mutexqueues.end())
    return VM_STATUS_ERROR_INVALID_ID;
  q = mutexqueues_it->second;

  if(ownerref == NULL)
    return VM_STATUS_ERROR_INVALID_PARAMETER;
  if(q == NULL)
    return VM_STATUS_ERROR_INVALID_STATE;

  if(q->empty())
    *ownerref = VM_THREAD_ID_INVALID;
  else
    *ownerref = q->front();

  return VM_STATUS_SUCCESS;
}

TVMStatus VMMutexAcquire(TVMMutexID mutex, TVMTick timeout)
{
  SignalGuard signalGuard(threadmanager->currentthread);
  signalGuard.lock();

  std::map<TVMMutexID, std::deque<TVMThreadID>* >::iterator mutexqueues_it =  mutexmanager->mutexqueues.find(mutex);
  std::deque<TVMThreadID>* q;

  sprintf(buf, "Acquiring mutex... [thread=%d, mutex=%d]", threadmanager->currentthread->id, mutex);
  status(buf);

  if(mutexqueues_it == mutexmanager->mutexqueues.end())
    return VM_STATUS_ERROR_INVALID_ID;
  q = mutexqueues_it->second;

  if(q == NULL)
    return VM_STATUS_ERROR_INVALID_STATE;

  if(q->empty()) {
    q->push_back(threadmanager->currentthread->getId());
    sprintf(buf, "Acquired mutex [thread=%d, mutex=%d]", threadmanager->currentthread->id, mutex);
    status(buf);
    return VM_STATUS_SUCCESS;
  }

  if(timeout == VM_TIMEOUT_IMMEDIATE) {
    return VM_STATUS_FAILURE;
  }
  else if(timeout == VM_TIMEOUT_INFINITE) {
    q->push_back(mutex);
    threadmanager->pushToWaiting(threadmanager->currentthread);
    threadmanager->replaceThread();
    if(q->front() != threadmanager->currentthread->getId()) {
      sprintf(buf, "Acquired mutex NOT? [thread=%d, mutex=%d]", threadmanager->currentthread->id, mutex);
      status(buf);
      return VM_STATUS_FAILURE;
    }
    sprintf(buf, "Acquired mutex [thread=%d, mutex=%d]", threadmanager->currentthread->id, mutex);
    status(buf);
    return VM_STATUS_SUCCESS;
  }
  else {
    /** TODO implement finite timeout **/
    return VM_STATUS_FAILURE;
  }

  return VM_STATUS_SUCCESS;
}

TVMStatus VMMutexRelease(TVMMutexID mutex)
{
  SignalGuard signalGuard(threadmanager->currentthread);
  signalGuard.lock();

  std::map<TVMMutexID, std::deque<TVMThreadID>* >::iterator mutexqueues_it =  mutexmanager->mutexqueues.find(mutex);
  std::deque<TVMThreadID>* q;

  sprintf(buf, "Released mutex [thread=%d, mutex=%d]", threadmanager->currentthread->id, mutex);
  status(buf);
  if(mutexqueues_it == mutexmanager->mutexqueues.end())
    return VM_STATUS_ERROR_INVALID_ID;
  q = mutexqueues_it->second;

  if(q == NULL ||
      q->empty() ||
      q->front() != threadmanager->currentthread->getId()
    )
    return VM_STATUS_ERROR_INVALID_STATE;

  q->pop_front();
  if(!q->empty()) {
    ThreadControlBlock* newthread = threadmanager->findThread(q->front());
    threadmanager->popFromWaiting(newthread);
    threadmanager->pushToReady(newthread);
    threadmanager->replaceThread();
  }
  sprintf(buf, "Released mutex [thread=%d, mutex=%d]", threadmanager->currentthread->id, mutex);
  status(buf);
  return VM_STATUS_SUCCESS;
}


/** VM FILE OPEN API **/


TVMStatus VMFileOpen(const char *filename, int flags, int mode, int *filedescriptor)
{
  if (!filename || !filedescriptor) return VM_STATUS_ERROR_INVALID_PARAMETER;

  TVMStatus filereturn = threadmanager->requestFileOpen(filename, flags, mode, filedescriptor);

  while(threadmanager->getCurrentThread()->getState() == VM_THREAD_STATE_WAITING);

  return filereturn;
}

TVMStatus VMFileClose(int filedescriptor)
{
  TVMStatus filereturn = threadmanager->requestFileClose(filedescriptor);

  while(threadmanager->getCurrentThread()->getState() == VM_THREAD_STATE_WAITING);

  return filereturn;
}

TVMStatus VMFileRead(int filedescriptor, void *data, int *length)
{
  if (!data || !length) return VM_STATUS_ERROR_INVALID_PARAMETER;

  TVMStatus filereturn = threadmanager->requestFileRead(filedescriptor, data, length);

  while(threadmanager->getCurrentThread()->getState() == VM_THREAD_STATE_WAITING);

  return filereturn;
}

TVMStatus VMFileWrite(int filedescriptor, void *data, int *length)
{
  if (!data || !length) return VM_STATUS_ERROR_INVALID_PARAMETER;

  TVMStatus filereturn = threadmanager->requestFileWrite(filedescriptor, data, length);

  while(threadmanager->getCurrentThread()->getState() == VM_THREAD_STATE_WAITING);

  return filereturn;
}

TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset)
{
  TVMStatus filereturn = threadmanager->requestFileSeek(filedescriptor, offset, whence, newoffset);

  while(threadmanager->getCurrentThread()->getState() == VM_THREAD_STATE_WAITING);

  return filereturn;
}


/** VM Memory API **/


TVMStatus VMMemoryPoolCreate(void *base, TVMMemorySize size, TVMMemoryPoolIDRef memory)
{
  if (!base || !size || !memory) return VM_STATUS_ERROR_INVALID_PARAMETER;

  *memory = memorymanager->createPool(base, size);

  return VM_STATUS_SUCCESS;
}

TVMStatus VMMemoryPoolDelete(TVMMemoryPoolID memory)
{
  return memorymanager->deletePool(memory);
}

TVMStatus VMMemoryPoolQuery(TVMMemoryPoolID memory, TVMMemorySizeRef bytesleft)
{
  if (!bytesleft) return VM_STATUS_ERROR_INVALID_PARAMETER;

  MemoryPool* pool = memorymanager->getPool(memory);

  if (!pool) return VM_STATUS_ERROR_INVALID_PARAMETER;

  *bytesleft = pool->query();

  return VM_STATUS_SUCCESS;
}

TVMStatus VMMemoryPoolAllocate(TVMMemoryPoolID memory, TVMMemorySize size, void **pointer)
{
  if (!size || !pointer) return VM_STATUS_ERROR_INVALID_PARAMETER;

  MemoryPool* pool = memorymanager->getPool(memory);

  if (!pool) return VM_STATUS_ERROR_INVALID_PARAMETER;

  return pool->allocate(size, pointer);
}

TVMStatus VMMemoryPoolDeallocate(TVMMemoryPoolID memory, void *pointer)
{
  if (!pointer) return VM_STATUS_ERROR_INVALID_PARAMETER;

  MemoryPool* pool = memorymanager->getPool(memory);

  if (!pool) return VM_STATUS_ERROR_INVALID_PARAMETER;

  return pool->deallocate(pointer);
}


/** VM Thread Scheduler **/

bool STDERR_DEBUG = false;

void printQueue(std::deque<ThreadControlBlock*>* q) {
  if (!STDERR_DEBUG) return;

  using namespace std;
  if(q==NULL)
    return;
  deque<ThreadControlBlock*>::iterator q_it;
  for(q_it = q->begin(); q_it != q->end(); q_it++)
    cerr << " " << (*q_it)->id;
  cerr << "\n";
}
void status(const char* msg)
{
  if (!STDERR_DEBUG) return;

  static int i =0;
  std::map<TVMMutexID, std::deque<TVMThreadID>* >::iterator mutexqueues_it;
  std::deque<TVMThreadID>* q;
  std::deque<TVMThreadID>::iterator q_it;

  using namespace std;
  cerr << ">>--------- "<<i<<" --------->>\n";
  cerr << ">> " << msg << "\n";
  cerr << "Queues...\n";
  cerr << "ready_low:";
  printQueue(&threadmanager->threadqueue_ready_low);
  cerr << "ready_med:";
  printQueue(&threadmanager->threadqueue_ready_med);
  cerr << "ready_high:";
  printQueue(&threadmanager->threadqueue_ready_high);
  cerr << "sleeping:";
  printQueue(&threadmanager->threadqueue_sleeping);
  cerr << "waiting:";
  printQueue(&threadmanager->threadqueue_waiting);
  cerr << "dead:";
  printQueue(&threadmanager->threadqueue_dead);

  cerr << "Threads...\n";
  for(unsigned int i = 0; i <= threadmanager->threadcounter; i++) {
    ThreadControlBlock* thread = threadmanager->findThread(i);
    if(!thread)
      continue;

    cerr << i << ": ";
    switch(thread->state){
      case VM_THREAD_STATE_DEAD:
        cerr << "DEAD\n";
        break;
      case VM_THREAD_STATE_RUNNING:
        cerr << "RUNNING\n";
        break;
      case VM_THREAD_STATE_READY:
        cerr << "READY\n";
        break;
      case VM_THREAD_STATE_WAITING:
        cerr << "WAITING\n";
        break;
    }

  }

  cerr << "Mutex...\n";
  for(mutexqueues_it = mutexmanager->mutexqueues.begin();
      mutexqueues_it != mutexmanager->mutexqueues.end();
      mutexqueues_it++)
  {
    cerr << mutexqueues_it->first << ":";

    q = mutexqueues_it->second;
    if(q == NULL) {
      cerr << " NULL\n";
      continue;
    }

    for(q_it = q->begin(); q_it != q->end(); q_it++) {
      cerr << " " << *q_it;
    }
    cerr << "\n";

  }
  cerr << "<<--------- "<<i<<" ---------<<\n";
  cerr << flush;
  i++;

}

void MachineAlarmCallback(void *calldata)
{
  threadmanager->updateSleepingThreads();
  threadmanager->replaceThread();

}

ThreadControlBlock::ThreadControlBlock(bool ismainthread):
  ismainthread(ismainthread),
  signalsOn(true),
  call(0),
  stackaddr(0),
  id(1),
  prio(VM_THREAD_PRIORITY_NORMAL),
  state(VM_THREAD_STATE_RUNNING)
{

}

ThreadControlBlock::ThreadControlBlock(TVMThreadEntry entry, void* param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadID id):
  ismainthread(false),
  signalsOn(true),
  entry(entry),
  param(param),
  call(0),
  stackaddr(0),
  stacksize(memsize),
  id(id),
  prio(prio),
  state(VM_THREAD_STATE_DEAD)
{

}

ThreadControlBlock::~ThreadControlBlock()
{
  delete call;
  memorymanager->getPool(VM_MEMORY_POOL_ID_SYSTEM)->deallocate(stackaddr);
}

TVMThreadID ThreadControlBlock::getId()
{
  return id;
}

TVMThreadState ThreadControlBlock::getState()
{
  return state;
}

TVMThreadPriority ThreadControlBlock::getPrio()
{
  return prio;
}

SMachineContextRef ThreadControlBlock::getMcnxtRef()
{
  return &mcnxt;
}

void ThreadControlBlock::enableSignals()
{
  MachineEnableSignals();
  signalsOn = true;
}

void ThreadControlBlock::disableSignals()
{
  MachineSuspendSignals(NULL);
  signalsOn = false;
}

void ThreadControlBlock::restoreSignals()
{
  if(signalsOn)
    enableSignals();
  else
    disableSignals();
}

void ThreadControlBlock::activate()
{
  state = VM_THREAD_STATE_READY;

  if (call) delete call;
  call = new skeletonCall();
  call->entry = entry;
  call->param = param;
  call->threadid = id;

  // if (stackaddr) delete[] stackaddr;
  // stackaddr = new char[stacksize];

  MemoryPool* sysmempool = memorymanager->getPool(VM_MEMORY_POOL_ID_SYSTEM);

  if (stackaddr) sysmempool->deallocate(stackaddr);
  sysmempool->allocate(stacksize, (void**) &stackaddr);

  MachineContextCreate(&mcnxt, skeletonEntry, call, stackaddr, stacksize);
}

void ThreadControlBlock::terminate()
{
  state = VM_THREAD_STATE_DEAD;
}

void ThreadControlBlock::dead()
{
  state = VM_THREAD_STATE_DEAD;
}

void ThreadControlBlock::sleep(TVMTick ticks)
{
  state = VM_THREAD_STATE_WAITING;

  sleepcounter = ticks;
}

void ThreadControlBlock::ready()
{
  state = VM_THREAD_STATE_READY;
}

void ThreadControlBlock::running()
{
  state = VM_THREAD_STATE_RUNNING;
}

void ThreadControlBlock::waiting()
{
  state = VM_THREAD_STATE_WAITING;
}

TVMTick ThreadControlBlock::getSleepcounter()
{
  return sleepcounter;
}

void ThreadControlBlock::updateSleepcounter()
{
  if (sleepcounter == VM_TIMEOUT_INFINITE) return;
  if (sleepcounter) --sleepcounter;
}

void ThreadControlBlock::skeletonEntry(void* call)
{
  skeletonCallRef _call = (skeletonCallRef) call;
  (_call->entry)(_call->param);

  VMThreadTerminate(_call->threadid);
}

  SignalGuard::SignalGuard(ThreadControlBlock *t)
: thread(t),
  released(false)
{
}

SignalGuard::~SignalGuard()
{
  if(!released)
    unlock();
}

void SignalGuard::lock()
{
  if(thread)
    thread->disableSignals();
}

void SignalGuard::unlock()
{
  if(thread)
    thread->enableSignals();
  released = true;
}

ThreadManager::ThreadManager():
  threadcounter(1)
{
  currentthread = new ThreadControlBlock(true);

  idlethread = new ThreadControlBlock(idleloop, NULL, 0x100000, VM_THREAD_PRIORITY_IDLE, 0);
  idlethread->activate();
}

ThreadManager::~ThreadManager()
{
  delete currentthread;
  delete idlethread;
}

TVMThreadID ThreadManager::getNewId()
{
  return ++threadcounter;
}

ThreadControlBlock* ThreadManager::getCurrentThread()
{
  return currentthread;
}

ThreadControlBlock* ThreadManager::findThread(TVMThreadID id)
{
  std::deque<ThreadControlBlock*>::iterator tcb_it;

  if (idlethread->getId() == id) return idlethread;
  if (currentthread->getId() == id) return currentthread;

  for (tcb_it = threadqueue_dead.begin(); tcb_it != threadqueue_dead.end(); ++tcb_it)
    if ((*tcb_it)->getId() == id) return *tcb_it;

  for (tcb_it = threadqueue_waiting.begin(); tcb_it != threadqueue_waiting.end(); ++tcb_it)
    if ((*tcb_it)->getId() == id) return *tcb_it;

  for (tcb_it = threadqueue_sleeping.begin(); tcb_it != threadqueue_sleeping.end(); ++tcb_it)
    if ((*tcb_it)->getId() == id) return *tcb_it;

  for (tcb_it = threadqueue_ready_low.begin(); tcb_it != threadqueue_ready_low.end(); ++tcb_it)
    if ((*tcb_it)->getId() == id) return *tcb_it;

  for (tcb_it = threadqueue_ready_med.begin(); tcb_it != threadqueue_ready_med.end(); ++tcb_it)
    if ((*tcb_it)->getId() == id) return *tcb_it;

  for (tcb_it = threadqueue_ready_high.begin(); tcb_it != threadqueue_ready_high.end(); ++tcb_it)
    if ((*tcb_it)->getId() == id) return *tcb_it;

  return 0;
}

TVMStatus ThreadManager::activateThread(TVMThreadID id)
{
  ThreadControlBlock* tcb_ptr = findThread(id);

  if (!tcb_ptr) return VM_STATUS_ERROR_INVALID_ID;
  if (tcb_ptr->getState() != VM_THREAD_STATE_DEAD) return VM_STATUS_ERROR_INVALID_STATE;

  tcb_ptr->activate();

  std::deque<ThreadControlBlock*>::iterator tcb_it;
  for (tcb_it = threadqueue_dead.begin(); tcb_it != threadqueue_dead.end() && *tcb_it != tcb_ptr; ++tcb_it);
  threadqueue_dead.erase(tcb_it);

  pushToReady(tcb_ptr);

  if (tcb_ptr->getPrio() > currentthread->getPrio()) replaceThread();

  return VM_STATUS_SUCCESS;
}

TVMStatus ThreadManager::terminateThread(TVMThreadID id)
{
  ThreadControlBlock* tcb_ptr = findThread(id);

  if (!tcb_ptr) return VM_STATUS_ERROR_INVALID_ID;
  if (tcb_ptr->getState() == VM_THREAD_STATE_DEAD) return VM_STATUS_ERROR_INVALID_STATE;

  tcb_ptr->terminate();

  popFromAll(tcb_ptr);
  threadqueue_dead.push_back(tcb_ptr);

  return VM_STATUS_SUCCESS;
}

void ThreadManager::replaceThread()
{
  SignalGuard signalGuard(threadmanager->currentthread);
  signalGuard.lock();

  ThreadControlBlock *newthread = 0, *oldthread;
  TVMThreadPriority currentprio = currentthread->getPrio();
  TVMThreadState currentstate = currentthread->getState();

  if ((currentprio != VM_THREAD_STATE_RUNNING || currentprio <=  VM_THREAD_PRIORITY_HIGH)
      && (!threadqueue_ready_high.empty()))
  {
    newthread = threadqueue_ready_high.front();
    threadqueue_ready_high.pop_front();
  }
  else if ((currentprio != VM_THREAD_STATE_RUNNING || currentprio <=  VM_THREAD_PRIORITY_NORMAL)
      && (!threadqueue_ready_med.empty()))
  {
    newthread = threadqueue_ready_med.front();
    threadqueue_ready_med.pop_front();
  }
  else if ((currentprio != VM_THREAD_STATE_RUNNING || currentprio <=  VM_THREAD_PRIORITY_LOW)
      && (!threadqueue_ready_low.empty()))
  {
    newthread = threadqueue_ready_low.front();
    threadqueue_ready_low.pop_front();
  }
  else if ((currentprio != VM_THREAD_STATE_RUNNING || currentprio > VM_THREAD_PRIORITY_IDLE)
      && (currentprio != VM_THREAD_STATE_RUNNING))
  {
    newthread = idlethread;
  } else return;

  if (currentstate == VM_THREAD_STATE_RUNNING) pushToReady(currentthread);
  newthread->running();
  oldthread = currentthread;
  currentthread = newthread;

  sprintf(buf, "replaceThread oldthread=%d, newthread=%d", oldthread->id, newthread->id);
  status(buf);

  currentthread->restoreSignals();
  MachineContextSwitch(oldthread->getMcnxtRef(), currentthread->getMcnxtRef());
  signalGuard.unlock();
}

void ThreadManager::popFromAll(ThreadControlBlock* thread)
{
  std::deque<ThreadControlBlock*>::iterator tcb_it;

  for (tcb_it = threadqueue_dead.begin(); tcb_it != threadqueue_dead.end();)
    if ((*tcb_it) == thread)
      tcb_it = threadqueue_dead.erase(tcb_it);
    else
      ++tcb_it;

  for (tcb_it = threadqueue_waiting.begin(); tcb_it != threadqueue_waiting.end();)
    if ((*tcb_it) == thread)
      tcb_it = threadqueue_waiting.erase(tcb_it);
    else
      ++tcb_it;

  for (tcb_it = threadqueue_sleeping.begin(); tcb_it != threadqueue_sleeping.end();)
    if ((*tcb_it) == thread)
      tcb_it = threadqueue_sleeping.erase(tcb_it);
    else
      ++tcb_it;

  for (tcb_it = threadqueue_ready_low.begin(); tcb_it != threadqueue_ready_low.end();)
    if ((*tcb_it) == thread)
      tcb_it = threadqueue_ready_low.erase(tcb_it);
    else
      ++tcb_it;

  for (tcb_it = threadqueue_ready_med.begin(); tcb_it != threadqueue_ready_med.end();)
    if ((*tcb_it) == thread)
      tcb_it = threadqueue_ready_med.erase(tcb_it);
    else
      ++tcb_it;

  for (tcb_it = threadqueue_ready_high.begin(); tcb_it != threadqueue_ready_high.end();)
    if ((*tcb_it) == thread)
      tcb_it = threadqueue_ready_high.erase(tcb_it);
    else
      ++tcb_it;
}

void ThreadManager::popFromWaiting(ThreadControlBlock* thread)
{
  for (std::deque<ThreadControlBlock*>::iterator tcb_it = threadqueue_waiting.begin(); tcb_it != threadqueue_waiting.end();)
    if ((*tcb_it) == thread)
      tcb_it = threadqueue_waiting.erase(tcb_it);
    else
      ++tcb_it;
}

void ThreadManager::pushToDead(ThreadControlBlock* thread)
{
  thread->dead();
  threadqueue_dead.push_back(thread);
}

void ThreadManager::pushToSleep(ThreadControlBlock* thread, TVMTick tick)
{
  currentthread->sleep(tick);
  threadqueue_sleeping.push_back(thread);
}

void ThreadManager::pushToReady(ThreadControlBlock* thread)
{
  thread->ready();
  switch(thread->getPrio())
  {
    case VM_THREAD_PRIORITY_LOW:
      threadqueue_ready_low.push_back(thread);
      break;
    case VM_THREAD_PRIORITY_NORMAL:
      threadqueue_ready_med.push_back(thread);
      break;
    case VM_THREAD_PRIORITY_HIGH:
      threadqueue_ready_high.push_back(thread);
      break;
    default:
      break;
  }
}

void ThreadManager::pushToWaiting(ThreadControlBlock* thread)
{
  thread->waiting();
  threadqueue_waiting.push_back(thread);
}

void ThreadManager::updateSleepingThreads()
{
  for (std::deque<ThreadControlBlock*>::iterator tcb_it = threadqueue_sleeping.begin(); tcb_it != threadqueue_sleeping.end();)
    if ((*tcb_it)->getSleepcounter())
    {
      (*tcb_it)->updateSleepcounter();
      ++tcb_it;
    }
    else
    {
      pushToReady(*tcb_it);
      tcb_it = threadqueue_sleeping.erase(tcb_it);
    }
}

void ThreadManager::idleloop(void* param)
{
  while(true);
}

typedef struct {
  ThreadManager *self;
  ThreadControlBlock* requestingthread;
  int result;
} requestFileOperationStruct, *requestFileOperationStructRef;

void ThreadManager::requestFileOperationCallback(void *calldata, int result)
{
  requestFileOperationStructRef _calldata = (requestFileOperationStructRef) calldata;

  _calldata->result = result;

  _calldata->self->popFromWaiting(_calldata->requestingthread);
  _calldata->self->pushToReady(_calldata->requestingthread);
  _calldata->self->replaceThread();
}

TVMStatus ThreadManager::requestFileOpen(const char *filename, int flags, int mode, int *filedescriptor)
{
  requestFileOperationStruct calldata;
  calldata.self = this;
  calldata.requestingthread = currentthread;

  pushToWaiting(currentthread);

  MachineFileOpen(filename, flags, mode, requestFileOperationCallback, &calldata);
  replaceThread();

  *filedescriptor = calldata.result;

  return (calldata.result >= 0) ? VM_STATUS_SUCCESS : VM_STATUS_FAILURE;
}

TVMStatus ThreadManager::requestFileWrite(int filedescriptor, void *data, int *length)
{
  requestFileOperationStruct calldata;
  calldata.self = this;
  calldata.requestingthread = currentthread;

  int bytesWritten = 0;
  while(bytesWritten < *length) {
    // could make decision more robust
    int bytesToWrite = ( *length - bytesWritten >= 512) ? 512 : ( *length - bytesWritten );

    memcpy(sharedmemory, (uint8_t*)data + bytesWritten, bytesToWrite);

    pushToWaiting(currentthread);
    MachineFileWrite(filedescriptor, sharedmemory, bytesToWrite, requestFileOperationCallback, &calldata);
    replaceThread();

    bytesWritten += calldata.result;
  }

  if(bytesWritten != *length)
    std::cout << "WTF!!!! bytesWriten != length\n" << std::flush;

  if (calldata.result >= 0) *length = bytesWritten;

  return (calldata.result >= 0) ? VM_STATUS_SUCCESS : VM_STATUS_FAILURE;
}

TVMStatus ThreadManager::requestFileSeek(int filedescriptor, int offset, int whence, int *newoffset)
{
  requestFileOperationStruct calldata;
  calldata.self = this;
  calldata.requestingthread = currentthread;

  pushToWaiting(currentthread);

  MachineFileSeek(filedescriptor, offset, whence, requestFileOperationCallback, &calldata);
  replaceThread();

  if (newoffset) *newoffset = calldata.result;

  return (calldata.result >= 0) ? VM_STATUS_SUCCESS : VM_STATUS_FAILURE;
}

TVMStatus ThreadManager::requestFileRead(int filedescriptor, void *data, int *length)
{
  requestFileOperationStruct calldata;
  calldata.self = this;
  calldata.requestingthread = currentthread;

  pushToWaiting(currentthread);

  int bytesRead = 0;
  while(bytesRead < *length) {
    int bytesToRead = ( *length - bytesRead >= 512 ) ? 512 : ( *length - bytesRead );

    pushToWaiting(currentthread);
    MachineFileWrite(filedescriptor, sharedmemory, bytesToRead, requestFileOperationCallback, &calldata);
    replaceThread();

    memcpy((uint8_t*)data + bytesRead, sharedmemory, calldata.result);

    bytesRead += calldata.result;
  }

  if(bytesRead != *length)
    std::cout << "WTF!!!! bytesRead != length\n" << std::flush;

  if (calldata.result >= 0) *length = bytesRead;

  return (calldata.result >= 0) ? VM_STATUS_SUCCESS : VM_STATUS_FAILURE;
}

TVMStatus ThreadManager::requestFileClose(int filedescriptor)
{
  requestFileOperationStruct calldata;
  calldata.self = this;
  calldata.requestingthread = currentthread;

  pushToWaiting(currentthread);

  MachineFileClose(filedescriptor, requestFileOperationCallback, &calldata);
  replaceThread();

  return (calldata.result >= 0) ? VM_STATUS_SUCCESS : VM_STATUS_FAILURE;
}

MutexManager::MutexManager() :
  lastID(0)
{
}

MutexManager::~MutexManager()
{
}
