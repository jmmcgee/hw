#include "VirtualMachine.h"


/** VM Thread API **/


TVMStatus VMStart(int tickms, int machinetickms, int argc, char *argv[])
{
  TVMMainEntry vmmain = VMLoadModule(argv[0]);

  if (!vmmain) return VM_STATUS_FAILURE;

  vmmain(argc,argv);
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
  return 0;
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
  return 0;
}

TVMStatus VMFileSeek(int filedescriptor, int offset, int whence, int *newoffset)
{
  return 0;
}
