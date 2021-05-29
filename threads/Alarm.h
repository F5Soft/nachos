// Alarm.h 
#ifndef ALARM_H
#define ALARM_H

#include "system.h"
#include "dllist.h"

void check(int which);

class Alarm {

    static Alarm *instance;
    static void new_instance();

  public:
    Alarm();
    ~Alarm();

    void Pause(int howLong);
    void CheckIfDue();
    
    int Getpausenum();
	
  private:
    DLList *queue;			
    int pausenum;
};

#endif // Alarm_H