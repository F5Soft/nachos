#include "Alarm.h"
#include "system.h"


//----------------------------------------------------------------------
// Alarm
// The following class defines a "Alarm"
//----------------------------------------------------------------------
Alarm::Alarm()
{
	queue = new DLList();
	pausenum = 0;
}

Alarm::~Alarm()
{
	delete queue;
}

//----------------------------------------------------------------------
// Alarm::Pause
// Threads call this function to go to sleep for a period of time.
//----------------------------------------------------------------------

/* 
	设定闹钟 睡眠howLong个时间单位 
	判断是否是唯一一个等待闹钟的线程 是则创建check线程
	把线程和唤醒时刻存入队列
	陷入睡眠
*/
void Alarm::Pause(int howLong, void check(int))
{
	ASSERT(howLong > 0);

	pausenum++;
	if (pausenum == 1)
	{
		Thread *t = new Thread("forked thread");
		t->Fork(check, 0);
	}

	IntStatus oldlevel = interrupt->SetLevel(IntOff);

	int lefttime = stats->totalTicks + howLong * TimerTicks;
	queue->SortedInsert((void *)currentThread, lefttime);
	currentThread->Sleep();

	(void)interrupt->SetLevel(oldlevel);
}

//----------------------------------------------------------------------
// Alarm::CheckIfDue
// called when interval has expired, bound in TimerInterruptHandler, system.cc
//----------------------------------------------------------------------

/* 
	唤醒闹钟 时间中断发生时调用
	遍历已存入队列的闹钟记录，如果到时则移除记录并唤醒
*/
void Alarm::CheckIfDue()
{
	int ptime = -1;
	Thread *thread = NULL;

	IntStatus oldLevel = interrupt->SetLevel(IntOff);

	thread = (Thread *)queue->Remove(&ptime);
	while (thread != NULL)
	{
		if (stats->totalTicks >= ptime)
		{
			scheduler->ReadyToRun(thread);
			pausenum--;
			thread = (Thread *)queue->Remove(&ptime);
		}
		else
		{
			queue->SortedInsert((void *)thread, ptime);
			(void)interrupt->SetLevel(oldLevel);
			return;
		}
	}
}

//----------------------------------------------------------------------
// Alarm::Getpausenum
//      Return pausenum
//----------------------------------------------------------------------

/* 返回阻塞线程数 */
int Alarm::Getpausenum()
{
	return pausenum;
}