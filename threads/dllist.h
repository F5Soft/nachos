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
    DLList(char *lockname); //加入lock
    ~DLList();

    void Prepend(void *item);
    void Append(void *item, int s, int t);
    void *Remove(int *keyPtr);
    bool IsEmpty();
    void SortedInsert(void *item, int sortKey, int testnum, int tid);
    void *SortedRemove(int sortKey, int testnum, int tid);

private:
    DLLElement *first; // head of the list, NULL if empty
    DLLElement *last;  // last element of the list, NULL if empty
    Lock *lock;
};

void CreateN(DLList *list, int n, int *back);
void RemoveN(DLList *list, int n);

#endif
