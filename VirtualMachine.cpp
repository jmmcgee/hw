#include <unistd.h>

#include <deque>

#include "Machine.h"
#include "VirtualMachine.h"



extern "C" {
  /** Forward Delcarations **/


  class ThreadControlBlock {
    public:
      bool ismainthread;

      SMachineContext mcnxt;

      TVMThreadEntry entry;
      void* param;

      void *stackaddr;
      TVMMemorySize stacksize;

      TVMTick sleepcounter;
      TVMThreadID id;
      TVMThreadPriority prio;
      TVMThreadState state;

      ThreadControlBlock(bool ismainthread);
      ThreadControlBlock(TVMThreadEntry entry, void* param, TVMMemorySize memsize, TVMThreadPriority prio);

      void decrementSleepCounter();
  };

  class ThreadManager {
    public:
      ThreadControlBlock *currentthread;

      std::deque<ThreadControlBlock*> threadqueue_ready_low;
      std::deque<ThreadControlBlock*> threadqueue_ready_med;
      std::deque<ThreadControlBlock*> threadqueue_ready_high;

      std::deque<ThreadControlBlock*> threadqueue_sleeping;
      std::deque<ThreadControlBlock*> threadqueue_waiting;
      std::deque<ThreadControlBlock*> threadqueue_dead;

      void decrementSleepcounters();
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

    // TODO: create special-case TCB for VMMain main thread.

    vmmain(argc,argv);

    MachineTerminate();

    return VM_STATUS_SUCCESS;
  }

  TVMStatus VMThreadCreate(TVMThreadEntry entry, void *param,
      TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadIDRef tid)
  {
    return 0;
  }

  TVMStatus VMThreadDelete(TVMThreadID thread)
  {
    return 0;
  }

  TVMStatus VMThreadActivate(TVMThreadID thread)
  {
    return 0;
  }

  TVMStatus VMThreadTerminate(TVMThreadID thread)
  {
    return 0;
  }

  TVMStatus VMThreadID(TVMThreadIDRef threadref)
  {
    return 0;
  }

  TVMStatus VMThreadState(TVMThreadID thread, TVMThreadStateRef stateref)
  {
    return 0;
  }

  TVMStatus VMThreadSleep(TVMTick tick)
  {
    if (tick == VM_TIMEOUT_INFINITE) return VM_STATUS_ERROR_INVALID_PARAMETER;

    threadmanager.currentthread->sleepcounter = tick;

    while(threadmanager.currentthread->sleepcounter);

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
    threadmanager.decrementSleepcounters();
  }

  ThreadControlBlock::ThreadControlBlock(bool ismainthread):
    ismainthread(ismainthread)
    prio(VM_THREAD_PRIORITY_NORMAL)
  {

  }

  ThreadControlBlock::ThreadControlBlock(TVMThreadEntry entry, void* param, TVMMemorySize memsize, TVMThreadPriority prio):
    ismainthread(false),
    entry(entry),
    param(param),
    stacksize(memsize),
    prio(prio)
  {

  }

  void ThreadControlBlock::decrementSleepCounter()
  {
    if (sleepcounter) --sleepcounter;
  }

  void ThreadManager::decrementSleepcounters()
  {
    for (std::deque<ThreadControlBlock*>::iterator tcb_it = threadqueue_sleeping.begin(); tcb_it != threadqueue_sleeping.end(); ++tcb_it)
      (*tcb_it)->decrementSleepCounter();
  }
}
