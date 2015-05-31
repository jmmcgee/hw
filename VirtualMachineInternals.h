#ifndef VIRTUALMACHINEINTERNALS_H
#define VIRTUALMACHINEINTERNALS_H

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <deque>
#include <queue>
#include <map>

#include "Machine.h"
#include "VirtualMachine.h"
#include "VirtualMachineMemory.h"


class MemoryManager;
class ThreadManager;
class MutexManager;

extern "C" TVMMainEntry VMLoadModule(const char *module);
extern MemoryManager *memorymanager;
extern ThreadManager *threadmanager;
extern MutexManager *mutexmanager;

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
  private:
    static ThreadManager* threadmanager;

    ThreadManager();
  public:
    ThreadControlBlock *currentthread;
    unsigned int threadcounter;

    std::deque<ThreadControlBlock*> threadqueue_ready_low;
    std::deque<ThreadControlBlock*> threadqueue_ready_med;
    std::deque<ThreadControlBlock*> threadqueue_ready_high;

    std::deque<ThreadControlBlock*> threadqueue_sleeping;
    std::deque<ThreadControlBlock*> threadqueue_waiting;
    std::deque<ThreadControlBlock*> threadqueue_dead;

    static ThreadManager* get();
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
  private:
    static MutexManager* mutexmanager;

    MutexManager();

  public:
    TVMMutexID lastID;
    std::map<TVMMutexID, std::deque<TVMThreadID>* > mutexqueues;

    static MutexManager* get(); 
    ~MutexManager();
};


void MachineAlarmCallback(void *calldata);
#endif
