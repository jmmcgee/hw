#include <unistd.h>
#include <iostream>

#include <deque>

#include "Machine.h"
#include "VirtualMachine.h"



extern "C" {
  /** Forward Delcarations **/

  typedef struct {
    TVMThreadEntry entry;
    void* param;
  } skeletonCall, *skeletonCallRef;

  class ThreadControlBlock {
    public:
      bool ismainthread;

      SMachineContext mcnxt;

      TVMThreadEntry entry;
      void* param;
      skeletonCallRef call;

      void *stackaddr;
      TVMMemorySize stacksize;

      volatile TVMTick sleepcounter;
      TVMThreadID id;
      TVMThreadPriority prio;
      TVMThreadState state;

      ThreadControlBlock(bool ismainthread);
      ThreadControlBlock(TVMThreadEntry entry, void* param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadID id);

      TVMThreadID getId();
      TVMThreadState getState();
      TVMThreadPriority getPrio();
      SMachineContextRef getMcnxtRef();

      void activate();
      void terminate();
      // TODO: make orthogonal ready() and sleep() methods to replace the below methods
      void setState(TVMThreadState state);
      void setSleepcounter(TVMTick ticks);
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

      TVMThreadID getNewId();

      ThreadControlBlock* getCurrentThread();

      void addThread(ThreadControlBlock *tcb_ptr);
      ThreadControlBlock* findThread(TVMThreadID id);

      TVMStatus activateThread(TVMThreadID id);
      TVMStatus terminateThread(TVMThreadID id);

      void replaceThread();
      void popFromAll(ThreadControlBlock* thread);
      void pushToSleep(ThreadControlBlock* thread);
      void pushToReady(ThreadControlBlock* thread);

      void updateSleepingThreads();


  };

  void MachineAlarmCallback(void *calldata);


  /** VM Globals **/


  ThreadManager threadmanager;


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

    *tid = threadmanager.getNewId();
    ThreadControlBlock *tcb_ptr = new ThreadControlBlock(entry, param, memsize, prio, *tid);

    threadmanager.addThread(tcb_ptr);

    return VM_STATUS_SUCCESS;
  }

  TVMStatus VMThreadDelete(TVMThreadID thread)
  {
    return 0;
  }

  TVMStatus VMThreadActivate(TVMThreadID thread)
  {
    return threadmanager.activateThread(thread);
  }

  TVMStatus VMThreadTerminate(TVMThreadID thread)
  {
    return threadmanager.terminateThread(thread);
  }

  TVMStatus VMThreadID(TVMThreadIDRef threadref)
  {
    return 0;
  }

  TVMStatus VMThreadState(TVMThreadID thread, TVMThreadStateRef stateref)
  {
    if (!stateref) return VM_STATUS_ERROR_INVALID_PARAMETER;

    ThreadControlBlock *tcb_ptr = threadmanager.findThread(thread);

    if (!tcb_ptr) return VM_STATUS_ERROR_INVALID_ID;

    *stateref = tcb_ptr->getState();

    return VM_STATUS_SUCCESS;
  }

  TVMStatus VMThreadSleep(TVMTick tick)
  {
    if (tick == VM_TIMEOUT_INFINITE) return VM_STATUS_ERROR_INVALID_PARAMETER;

    threadmanager.getCurrentThread()->setSleepcounter(tick);
    threadmanager.pushToSleep(threadmanager.getCurrentThread());

    if (tick == VM_TIMEOUT_IMMEDIATE) threadmanager.replaceThread();

    while(threadmanager.getCurrentThread()->getSleepcounter());

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
    return 0;
  }

  TVMStatus VMFileClose(int filedescriptor)
  {
    return 0;
  }

  TVMStatus VMFileRead(int filedescriptor, void *data, int *length)
  {
    return 0;
  }

  TVMStatus VMFileWrite(int filedescriptor, void *data, int *length)
  {
    ssize_t bytes_written;

    if (!data || !length) return VM_STATUS_ERROR_INVALID_PARAMETER;

    bytes_written = write(filedescriptor, data, *length);

    if (bytes_written == -1) return VM_STATUS_FAILURE;

    *length = *length - bytes_written;

    return VM_STATUS_SUCCESS;
  }

  TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset)
  {
    return 0;
  }


  /** VM Thread Scheduler **/


  void MachineAlarmCallback(void *calldata)
  {
    threadmanager.updateSleepingThreads();
    threadmanager.replaceThread();
  }

  ThreadControlBlock::ThreadControlBlock(bool ismainthread):
    ismainthread(ismainthread),
    id(0),
    prio(VM_THREAD_PRIORITY_NORMAL),
    state(VM_THREAD_STATE_RUNNING)
  {

  }

  ThreadControlBlock::ThreadControlBlock(TVMThreadEntry entry, void* param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadID id):
    ismainthread(false),
    entry(entry),
    param(param),
    stacksize(memsize),
    id(id),
    prio(prio),
    state(VM_THREAD_STATE_DEAD)
  {

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

    call = new skeletonCall();
    call->entry = entry;
    call->param = param;

    stackaddr = (void *) (new uint8_t[stacksize]);

    MachineContextCreate(&mcnxt, skeletonEntry, call, stackaddr, stacksize);
  }


  void ThreadControlBlock::terminate()
  {
    state = VM_THREAD_STATE_DEAD;

    // TODO: free skeleton call and allocated stack
  }

  void ThreadControlBlock::setState(TVMThreadState state)
  {
    this->state = state;
  }

  void ThreadControlBlock::setSleepcounter(TVMTick ticks)
  {
    state = VM_THREAD_STATE_WAITING;

    sleepcounter = ticks;
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

    VMThreadTerminate(threadmanager.getCurrentThread()->getId());
  }

  ThreadManager::ThreadManager():
    threadcounter(0)
  {
    currentthread = new ThreadControlBlock(true);
  }

  TVMThreadID ThreadManager::getNewId()
  {
    return ++threadcounter;
  }

  ThreadControlBlock* ThreadManager::getCurrentThread()
  {
    return currentthread;
  }

  void ThreadManager::addThread(ThreadControlBlock* tcb_ref)
  {
    threadqueue_dead.push_back(tcb_ref);
  }

  ThreadControlBlock* ThreadManager::findThread(TVMThreadID id)
  {
    std::deque<ThreadControlBlock*>::iterator tcb_it;

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

    TVMThreadPriority prio = tcb_ptr->getPrio();
    switch(prio)
    {
      case VM_THREAD_PRIORITY_LOW:
        threadqueue_ready_low.push_back(tcb_ptr);
        break;
      case VM_THREAD_PRIORITY_NORMAL:
        threadqueue_ready_med.push_back(tcb_ptr);
        break;
      case VM_THREAD_PRIORITY_HIGH:
        threadqueue_ready_high.push_back(tcb_ptr);
        break;
      default:
        break;
    }

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
    ThreadControlBlock* oldthread = currentthread;

    if (!threadqueue_ready_high.empty())
    {
      currentthread = threadqueue_ready_high.front();
      threadqueue_ready_high.pop_front();
    }
    else if (!threadqueue_ready_med.empty())
    {
      currentthread = threadqueue_ready_med.front();
      threadqueue_ready_med.pop_front();
    }
    else if (!threadqueue_ready_low.empty())
    {
      currentthread = threadqueue_ready_low.front();
      threadqueue_ready_low.pop_front();
    }

    if (oldthread == currentthread) return;

    std::cout << "CONTEXT SWITCH from " << oldthread << " to " << currentthread << std::endl;

    currentthread->setState(VM_THREAD_STATE_RUNNING);
    MachineContextSwitch(oldthread->getMcnxtRef(), currentthread->getMcnxtRef());
  }

  void ThreadManager::popFromAll(ThreadControlBlock* thread)
  {
    // TODO: implement this
  }

  void ThreadManager::pushToSleep(ThreadControlBlock* thread)
  {
    thread->setState(VM_THREAD_STATE_WAITING);
    threadqueue_sleeping.push_back(thread);
  }

  void ThreadManager::pushToReady(ThreadControlBlock* thread)
  {
    thread->setState(VM_THREAD_STATE_READY);
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
        std::cout << "FINISHED SLEEP " << *tcb_it << std::endl;

        pushToReady(*tcb_it);
        tcb_it = threadqueue_sleeping.erase(tcb_it);
      }
  }
}
