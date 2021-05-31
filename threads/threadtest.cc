#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "EventBarrier.h"
#include "dllist.h"
#include "BoundBuffer.h"
//sleep会唤醒就绪态进程

#include "Table.h"
#include "Alarm.h"
#include <sys/time.h>

int testnum = 0;
int N = 5;
int T = 5;

BoundedBuffer *buffer = new BoundedBuffer(10);
Table *table = new Table(10);
EventBarrier *Test_Barrier=new EventBarrier("test");

extern Alarm* alarm;

void BoundBufferTest(int tid)
{
    char *data;
    if (tid == 0)
    {
        data = new char[30];
        buffer->Read(data, 29);
        data[29] = '\0';
        printf("Data read at thread 0: %s\n", data);
    }
    if (tid == 1)
    {
        data = "Hello, world! ";
        printf("Data written at thread 1: %s\n", data);
        buffer->Write(data, strlen(data));
    }
    if (tid == 2)
    {
        data = new char[10];
        buffer->Read(data, 9);
        data[9] = '\0';
        printf("Data read at thread 2: %s\n", data);
    }
    if (tid == 3)
    {
        data = "And hello, world! again!";
        printf("Data written at thread 3: %s\n", data);
        buffer->Write(data, strlen(data));
    }
}

int getRandom()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return ((tv.tv_usec * 2 + 1) % 100);
}
void TableTest(int tid)
{
    int elem = getRandom();
    void *obj = (void *)elem;
    int position;

    position = table->Alloc(obj);
    if (position != -1)
    {
        elem = (int)table->Get(position);
        table->Release(position);
    }
}

/*
	创建一个线程，反复检查当前有几个等待闹钟的线程
	若为零则结束，若不为零则切换
*/
// A dummy function, just to cause idle.
void Check(int tid)
{
	while (alarm->Getpausenum() > 0)
		currentThread->Yield();
	currentThread->Finish();
}

void AlarmTest(int which)
{
    int howlong = Random() % 10000;
    printf("thread %d is waiting %d Tick now=%d\n", which, howlong, stats->totalTicks);
    alarm->Pause(howlong, Check);
    printf("thread %d is ok now=%d\n", which, stats->totalTicks);
}

void BarrierTest(int tid){
    if(tid==0){   //线程0模拟控制线程
        while(Test_Barrier->Waiters()<4){
            
            //  printf("%d\n", Test_Barrier->Waiters());
            currentThread->Yield();
        }
        Test_Barrier->Signal();
        printf("threads 0 is wake up\n");
    }
    else{
        
        Test_Barrier->Wait();
        printf("threads %d is wake up\n",tid);
    }
}

void ThreadTest()
{
    Thread *t;
    switch (testnum)
    {
    case 7:
        printf("Thread 0 and 2 wants to read. Thread 1 and 3 wants to write.\nAnd thread 0 runs first\n");
        for (int i = 1; i <= 3; i++)
        {
            t = new Thread("forked thread");
            t->Fork(BoundBufferTest, i);
        }
        BoundBufferTest(0);
        break;
    case 8:
        printf("Init a 10 slots table.\nThread 0 to T want to insert data into the table and then remove it\n");
        for (int i = 1; i <= T; i++)
        {
            t = new Thread("fork thread");
            t -> Fork(TableTest, i);
        }
        TableTest(0);
        break;
    case 9:
        printf("...\n");
        for (int i = 1; i < N; i++)
        {
            Thread *t = new Thread("fork thread");
            t->Fork(AlarmTest, i);
        }
        AlarmTest(0);
        break;
    case 10:
        for (int i = 1;i <= 4; i++)
        {
            t = new Thread("fork thread");
            t -> Fork(BarrierTest, i);
        }
        BarrierTest(0);
        break;
    default:
        printf("No test specified.\n");
        break;
    }
}
