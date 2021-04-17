#include "copyright.h"
#include "system.h"
#include "synch.h"

#include "dllist.h"
#include "BoundBuffer.h"
//sleep会唤醒就绪态进程

int testnum = 0;
int N = 5;
int T = 5;
// Condition* con=new Condition("c1");
Lock *lock = new Lock("l2");

DLList *list = new DLList("l1");
BoundedBuffer *buffer = new BoundedBuffer(10);

void Test(int tid)
{
    int keys[N];
    CreateN(list, N, keys);
    RemoveN(list, N);
}

void Error1(int tid)
{
    int *keys = new int[N];
    if (tid == 0)
    {
        lock->Acquire();
        for (int i = 0; i < N; i++)
        {
            CreateN(list, 1, keys + i);
            printf("thread %d created %d\n", tid, keys[i]);
            if (i == 1)
            {
                printf("Thread is preempted\n");
                currentThread->Yield();
            }
            if (i == 2)
                RemoveN(list, 3);
        }
        lock->Release();
        //con->Signal(lock);
    }
    if (tid == 1)
    {

        lock->Acquire();
        // lock->GetValue();
        //con->Wait(lock);   //两个都处于sleep状态直接结束
        for (int i = 0; i < N; i++)
        {
            CreateN(list, 1, keys + i);
            printf("thread %d created %d\n", tid, keys[i]);
            if (i == 2)
            {
                printf("Thread is preempted\n");
                currentThread->Yield();
            }
        }
        lock->Release();
        //con->Signal(lock);
    }
}

void Error2(int tid)
{

    int *keys = new int[N];
    if (tid == 0)
    {
        lock->Acquire();
        for (int i = 0; i < N; i++)
        {
            CreateN(list, 1, keys + i);
            printf("thread %d created %d\n", tid, keys[i]);
        }
        lock->Release();
        currentThread->Yield();
    }
    if (tid == 1)
    {
        lock->Acquire();
        for (int i = 0; i < N; i++)
        {
            CreateN(list, 1, keys + i);
            printf("thread %d created %d\n", tid, keys[i]);
        }
        lock->Release();
    }
}

void Error3(int tid)
{
    int *keys = new int[N];
    if (tid == 0)
    {
        lock->Acquire();
        for (int i = 0; i < N; i++)
        {
            CreateN(list, 1, keys + i);
            printf("thread %d created %d\n", tid, keys[i]);
            if (i == 1)
            {
                printf("Thread is preempted\n");
                currentThread->Yield();
            }
        }
        lock->Release();
    }
    if (tid == 1)
    {
        lock->Acquire();
        RemoveN(list, N);
        printf("Thread is preempted\n");
        currentThread->Yield();
        lock->Release();
    }
}

void Error4(int tid)
{
    if (tid == 0)
    {
        lock->Acquire();
        printf("thread 0 want to append point to list\n");
        list->Append(NULL, 4, 0);
        lock->Release();
        currentThread->Yield(); //强制切换
    }
    if (tid == 1)
    {
        lock->Acquire();
        printf("thread 1 want to append point to list\n");
        list->Append(NULL, 4, 1);
        printf("Thread is preempted\n");
        lock->Release();
        currentThread->Yield(); //强制切换
    }
}

void Error5(int tid)
{
    if (tid == 0)
    {
        printf("thread 0 want to remove point 4\n");
        list->SortedRemove(4, 5, 0);
        currentThread->Yield();
    }
    if (tid == 1)
    {
        printf("thread 1 want to remove point 2\n");
        list->SortedRemove(2, 5, 1);
        printf("Thread is preempted\n");
        currentThread->Yield();
    }
}

void Error6(int tid)
{
    if (tid == 0)
    {
        printf("thread 0 want to insert point 2\n");
        list->SortedInsert(NULL, 2, 6, 0);
        currentThread->Yield();
    }
    if (tid == 1)
    {
        printf("thread 1 want to remove point 0\n");
        list->SortedRemove(0, 5, 1);
        printf("Thread is preempted\n");
        currentThread->Yield();
    }
}

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

void ThreadTest()
{
    Thread *t;
    switch (testnum)
    {
    case 0:
        for (int i = 0; i < T; i++)
        {
            t = new Thread("forked thread");
            t->Fork(Test, i);
        }
        printf("Thread is preempted\n");
        currentThread->Yield();
        if (list->IsEmpty())
            printf("List is empty\n");
        break;
    case 1:
        printf("Thread 0 wants to created %d points and remove top 3 points.\nThread 1 wants to created %d points.\n", N, N);

        t = new Thread("forked thread");
        t->Fork(Error1, 1);
        Error1(0);
        break;
    case 2:
        printf("Thread 0 wants to created %d points.\nThread 1 wants to remove %d points.\nFinally Thread 0 removed all points.\n", N, N);

        t = new Thread("forked thread");
        t->Fork(Error2, 1);
        Error2(0);
        RemoveN(list, 2 * N);
        break;
    case 3:
        printf("Thread 0 wants to created %d points.\nThread 1 wants to remove %d points.\n", N, N);

        t = new Thread("forked thread");
        t->Fork(Error3, 1);
        Error3(0);
        break;
    case 4:

        t = new Thread("forked thread");
        t->Fork(Error4, 1);
        Error4(0);
        RemoveN(list, 2);
        break;
    case 5:
        for (int i = 0; i < 8; i++)
            list->Append(NULL, 0, 0);

        t = new Thread("forked thread");
        t->Fork(Error5, 1);
        Error5(0);
        RemoveN(list, 8);
        break;
    case 6:
        for (int i = 0; i < 8; i++)
            list->Append(NULL, 0, 0);

        t = new Thread("forked thread");
        t->Fork(Error6, 1);
        Error6(0);
        RemoveN(list, 8);
        printf("The list is not in order!\n");
        break;
    case 8:
        printf("Thread 0 and 2 wants to read. Thread 1 and 3 wants to write.\nAnd thread 0 runs first\n");
        for (int i = 1; i <= 3; i++)
        {
            t = new Thread("forked thread");
            t->Fork(BoundBufferTest, i);
        }
        BoundBufferTest(0);
    default:
        printf("No test specified.\n");
        break;
    }
}
