#include <unistd.h>

#include <vector>

#include "Machine.h"
#include "VirtualMachine.h"


/** VM Thread API **/

extern "C" {
  class ThreadControlBlock {
    public:
      SMachineContext mcnxt;
      void *stackaddr;
      TVMMemorySize stacksize;

      TVMTick sleepcounter;
      TVMThreadID id;
      TVMThreadPriority prio;
      TVMThreadState state;

  }

  volatile TVMTick sleepCounter = 0;

  void MachineAlarmCallback(void *calldata);

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

    sleepCounter = tick;

    while(sleepCounter);

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

  void MachineAlarmCallback(void *calldata)
  {
    if (sleepCounter) --sleepCounter;
  }
}
