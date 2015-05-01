#include <unistd.h>
#include <iostream>

#include <deque>

#include "Machine.h"
#include "VirtualMachine.h"



extern "C" {
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

  void MachineAlarmCallback(void *calldata);


  /** VM Globals **/


  ThreadManager *threadmanager = new ThreadManager;


  /** VM Thread API **/


  TVMStatus VMStart(int tickms, int machinetickms, int argc, char *argv[])
  {
    MachineInitialize(machinetickms);
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

    *tid = threadmanager->getNewId();
    ThreadControlBlock *tcb_ptr = new ThreadControlBlock(entry, param, memsize, prio, *tid);

    threadmanager->pushToDead(tcb_ptr);

    return VM_STATUS_SUCCESS;
  }

  TVMStatus VMThreadDelete(TVMThreadID thread)
  {
    return 0;
  }

  TVMStatus VMThreadActivate(TVMThreadID thread)
  {
    return threadmanager->activateThread(thread);
  }

  TVMStatus VMThreadTerminate(TVMThreadID thread)
  {
    TVMStatus terminatereturn = threadmanager->terminateThread(thread);

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

    threadmanager->pushToSleep(threadmanager->getCurrentThread(), tick);

    if (tick == VM_TIMEOUT_IMMEDIATE) threadmanager->replaceThread();

    while(threadmanager->getCurrentThread()->getSleepcounter());

    return VM_STATUS_SUCCESS;
  }


  /** VM Mutex API **/


  TVMStatus VMMutexCreate(TVMMutexIDRef mutexref)
  {
    return 0;
  }

  TVMStatus VMMutexDelete(TVMMutexID mutex)
  {
    return 0;
  }

  TVMStatus VMMutexQuery(TVMMutexID mutex, TVMThreadIDRef ownerref)
  {
    return 0;
  }

  TVMStatus VMMutexAcquire(TVMMutexID mutex, TVMTick timeout)
  {
    return 0;
  }

  TVMStatus VMMutexRelease(TVMMutexID mutex)
  {
    return 0;
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


  /** VM Thread Scheduler **/


  void MachineAlarmCallback(void *calldata)
  {
    threadmanager->updateSleepingThreads();
    threadmanager->replaceThread();
  }

  ThreadControlBlock::ThreadControlBlock(bool ismainthread):
    ismainthread(ismainthread),
    call(0),
    stackaddr(0),
    id(1),
    prio(VM_THREAD_PRIORITY_NORMAL),
    state(VM_THREAD_STATE_RUNNING)
  {

  }

  ThreadControlBlock::ThreadControlBlock(TVMThreadEntry entry, void* param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadID id):
    ismainthread(false),
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
    delete[] stackaddr;
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

  void ThreadControlBlock::activate()
  {
    state = VM_THREAD_STATE_READY;

    if (call) delete call;
    call = new skeletonCall();
    call->entry = entry;
    call->param = param;
    call->threadid = id;

    if (stackaddr) delete[] stackaddr;
    stackaddr = new char[stacksize];

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
    if (sleepcounter) --sleepcounter;
  }

  void ThreadControlBlock::skeletonEntry(void* call)
  {
    skeletonCallRef _call = (skeletonCallRef) call;
    (_call->entry)(_call->param);

    VMThreadTerminate(_call->threadid);
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
    ThreadControlBlock *newthread = 0, *oldthread;
    TVMThreadPriority currentprio = currentthread->getPrio();
    TVMThreadState currentstate = currentthread->getState();

    if ((currentprio <=  VM_THREAD_PRIORITY_HIGH) && (!threadqueue_ready_high.empty()))
    {
      newthread = threadqueue_ready_high.front();
      threadqueue_ready_high.pop_front();
    }
    else if ((currentprio <=  VM_THREAD_PRIORITY_NORMAL) && (!threadqueue_ready_med.empty()))
    {
      newthread = threadqueue_ready_med.front();
      threadqueue_ready_med.pop_front();
    }
    else if ((currentprio <=  VM_THREAD_PRIORITY_LOW) && (!threadqueue_ready_low.empty()))
    {
      newthread = threadqueue_ready_low.front();
      threadqueue_ready_low.pop_front();
    }
    else if ((currentprio > VM_THREAD_PRIORITY_IDLE) && (currentprio != VM_THREAD_STATE_RUNNING))
    {
      newthread = idlethread;
    } else return;

    if (currentstate == VM_THREAD_STATE_RUNNING) pushToReady(currentthread);
    newthread->running();
    oldthread = currentthread;
    currentthread = newthread;

    MachineContextSwitch(oldthread->getMcnxtRef(), currentthread->getMcnxtRef());
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

    pushToWaiting(currentthread);

    MachineFileWrite(filedescriptor, data, *length, requestFileOperationCallback, &calldata);
    replaceThread();

    if (calldata.result >= 0) *length = *length - calldata.result;

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

    MachineFileRead(filedescriptor, data, *length, requestFileOperationCallback, &calldata);
    replaceThread();

    if (calldata.result >= 0) *length = *length - calldata.result;

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
}
