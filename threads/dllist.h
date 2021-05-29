#ifndef DLLIST_H
#define DLLIST_H

#include "synch.h"

class DLLElement
{
public:
    DLLElement(void *itemPtr, int sortKey); // initialize a list element
    DLLElement *next;
    DLLElement *prev;
    int key;
    void *item;
};

class DLList
{
public:
    DLList(); //加入lock
    ~DLList();

    void Prepend(void *item);
    void Append(void *item);
    void *Remove(int *keyPtr);
    bool IsEmpty();
    void SortedInsert(void *item, int sortKey);
    void *SortedRemove(int sortKey);

private:
    DLLElement *first; // head of the list, NULL if empty
    DLLElement *last;  // last element of the list, NULL if empty
    Lock *lock;
};
#endif
