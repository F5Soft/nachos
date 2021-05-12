// synch.cc 
//  Routines for synchronizing threads.  Three kinds of
//  synchronization routines are defined here: semaphores, locks 
//      and condition variables (the implementation of the last two
//  are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
//  Initialize a semaphore, so that it can be used for synchronization.
//
//  "debugName" is an arbitrary name, useful for debugging.
//  "initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;   //信号量名字
    value = initialValue;   //初始值
    queue = new List;   //队列
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
//  De-allocate semaphore, when no longer needed.  Assume no one
//  is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
//  Wait until semaphore value > 0, then decrement.  Checking the
//  value and decrementing must be done atomically, so we
//  need to disable interrupts before checking the value.
//
//  Note that Thread::Sleep assumes that interrupts are disabled
//  when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()      //获取锁
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);   // disable interrupts
    
    while (value == 0) {            // semaphore not available
    queue->Append((void *)currentThread);   // so go to sleep
    currentThread->Sleep();
    } 
    value--;                    // semaphore available, 
                        // consume its value
    
    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
//  Increment semaphore value, waking up a waiter if necessary.
//  As with P(), this operation must be atomic, so we need to disable
//  interrupts.  Scheduler::ReadyToRun() assumes that threads
//  are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()     //释放锁
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)    // make thread ready, consuming the V immediately
    scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(char* debugName) {
    name=debugName;
    value=1;
    queue = new List;   //队列
    helder=NULL;
}
Lock::~Lock() {
    delete queue;
}
//测试用
void Lock::GetValue(){
    printf("Value is %d\n", value);
}
void Lock::Acquire() {
    ASSERT(!isHeldByCurrentThread());
    IntStatus oldLevel = interrupt->SetLevel(IntOff);   // 关中断
    
    while (value == 0) {            
    queue->Append((void *)currentThread);   
    currentThread->Sleep();
    } 
    value=0;                    
    helder=currentThread;                    
    
    (void) interrupt->SetLevel(oldLevel);   //开中断
}
void Lock::Release() {
    ASSERT(isHeldByCurrentThread());
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)    // make thread ready, consuming the V immediately
        scheduler->ReadyToRun(thread);
    value=1;
    helder=NULL;
    (void) interrupt->SetLevel(oldLevel);
}
bool Lock::isHeldByCurrentThread()   // true if the current thread
{
    return (currentThread==helder);
}
Condition::Condition(char* debugName) { 
    name=debugName;
    queue = new List;
}
Condition::~Condition() { 
    delete queue;
}
/*
功能 阻塞一个等待条件变量
     a 阻塞等待条件变量cond满足
     b 释放己掌握的互斥锁，相当于pthread_mutex_unlock(&mutex);
       a,b两步为一个原子操作
     c 当被唤醒，pthread_cond_wait函数返回时，
       解除阻塞并重新申请获取互斥锁 pthread_mutex_lock(&mutex)

参数 cond 指向要初始化的条件变量指针，mutex 互斥锁
返回 成功 0     失败 非0错误码
*/
void Condition::Wait(Lock* conditionLock) { 
    //ASSERT(FALSE);
    //conditionLock->Acquire();   //锁跟中断两个都要？  
    ASSERT(conditionLock->isHeldByCurrentThread());
    lock = conditionLock;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    conditionLock->Release();
    queue->Append((void *)currentThread);   // so go to sleep
    currentThread->Sleep();

    conditionLock->Acquire();
    (void) interrupt->SetLevel(oldLevel);
}
void Condition::Signal(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread() && lock == conditionLock);
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    thread = (Thread *)queue->Remove();
    if (thread != NULL)    // make thread ready, consuming the V immediately
        scheduler->ReadyToRun(thread);
    (void) interrupt->SetLevel(oldLevel);
}
void Condition::Broadcast(Lock* conditionLock) {
    ASSERT(conditionLock->isHeldByCurrentThread() && lock == conditionLock);
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    thread = (Thread *)queue->Remove();
    while(thread != NULL)    // make thread ready, consuming the V immediately
    {
        scheduler->ReadyToRun(thread);
        thread = (Thread *)queue->Remove();
    }
    (void) interrupt->SetLevel(oldLevel);
}
