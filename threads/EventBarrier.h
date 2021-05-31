#include "copyright.h"
#include "thread.h"
#include "list.h"
#include "synch.h"
#include "system.h"
#define SIGNALED 1
#define UNSIGNALED 0

class EventBarrier
{
public:
	EventBarrier(char *DebugName);
	~EventBarrier();
	void Signal();
	void Wait();
	void Complete();
	int Waiters();
private:
	char *Name;
	int State;
	int SigNum;
	int WaitNum;
	Semaphore *Finish;
	Semaphore *WakeNum;
	Lock *sMutex;
	Lock *wMutex;
	List *queue;

};
