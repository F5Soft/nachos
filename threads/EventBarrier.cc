#include "EventBarrier.h"
#include <string.h>
EventBarrier::EventBarrier(char *DebugName){
	Name = DebugName;
	State = UNSIGNALED;
	WaitNum = 0;
    SigNum = 0; 
    char num[50];
    strcpy(num,DebugName);
	Finish = new Semaphore(num,0);
    strcat(num,"1");
    WakeNum = new Semaphore(num,0);
    strcat(num,"1");
    sMutex=new Lock(num);
    strcat(num,"1");
    wMutex=new Lock(num);
    queue=new List;
}
EventBarrier::~EventBarrier(){
	delete Finish;
    delete WakeNum;
    delete sMutex;
    delete wMutex;
}
void 
EventBarrier::Signal(){
	Thread *thread;
    
    sMutex->Acquire();  //同一时刻只有一个调用Signal;可被Wait打断？，空队列时不会出现两个Signal
    
    State=SIGNALED;
    
    thread = (Thread *)queue->Remove();
    if(thread==NULL)
    {
    	SigNum++;
    	while(SigNum==1){
    		Finish->P();
    	}
    	State=UNSIGNALED;
    	return;
    }
    while(thread != NULL)    // make thread ready, consuming the V immediately
    {
        
    	WakeNum->V();
        thread = (Thread *)queue->Remove();
    }
    
    Finish->P();
    State=UNSIGNALED;
    sMutex->Release();
}
void
EventBarrier::Wait(){
	wMutex->Acquire();
	if(SigNum==1){               //空队列时情况
		SigNum--;
		Finish->V();
		return;
	}
    queue->Append((void *)currentThread);
    WaitNum++;
    wMutex->Release();
	WakeNum->P();
    Complete();
}
void
EventBarrier::Complete(){
	WaitNum--;   //互斥？
	if(WaitNum==0){
		Finish->V();
	}
}
 int 
 EventBarrier::Waiters(){
 	return WaitNum;
 }
