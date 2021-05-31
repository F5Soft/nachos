// Alarm.h 
#ifndef ALARM_H
#define ALARM_H

#include "system.h"
#include "dllist.h"

void Check(int tid);

class Alarm {

  public:
    Alarm();
    ~Alarm();

    void Pause(int howLong, void check(int));
    void CheckIfDue();
    int Getpausenum();

  private:
    DLList *queue;			
    int pausenum;
};

#endif // Alarm_H