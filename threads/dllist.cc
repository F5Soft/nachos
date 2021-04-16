#include "dllist.h"
#include "system.h"

DLLElement::DLLElement(void *itemPtr, int sortKey)
{
    item = itemPtr;
    key = sortKey;
    next = NULL;
    prev = NULL;
}

DLList::DLList(char *lockname)
{
    first = NULL;
    last = NULL;
    lock = new Lock(lockname);
}

DLList::~DLList()
{
    while (first != NULL)
        Remove(NULL);
    lock->~Lock();
}

void DLList::Prepend(void *item)
{
    lock->Acquire();
    DLLElement *elem;
    if (first == NULL)
    {
        elem = new DLLElement(item, 0);
        first = last = elem;
    }
    else
    {
        elem = new DLLElement(item, first->key - 1);
        elem->next = first;
        first->prev = elem;
        first = elem;
    }
    lock->Release();
}

void DLList::Append(void *item, int testnum, int tid)
{
    lock->Acquire();
    DLLElement *elem;
    if (last == NULL)
    {
        if (testnum == 4 && tid == 0)
        {
            printf("Thread is preempted\n");
            currentThread->Yield();
        }
        elem = new DLLElement(item, 0);
        first = last = elem;
    }
    else
    {
        elem = new DLLElement(item, last->key + 1);
        elem->prev = last;
        last->next = elem;
        last = elem;
    }
    printf("Thread %d append point whose value is %d\n", tid, elem->key);
    lock->Release();
}

void *DLList::Remove(int *keyPtr)
{
    lock->Acquire();
    DLLElement *elem = first;
    if (elem == NULL)
    {
        lock->Release();
        return NULL;
    }
    void *item = elem->item;
    if (keyPtr != NULL)
        *keyPtr = elem->key;
    if (elem->next != NULL)
        elem->next->prev = NULL;
    else
        last = NULL;
    first = elem->next;
    delete elem;
    lock->Release();
    return item;
}

bool DLList::IsEmpty()
{
    return first == NULL && last == NULL;
}

void DLList::SortedInsert(void *item, int sortKey, int testnum, int tid)
{
    lock->Acquire();
    DLLElement *pos = first;
    DLLElement *elem = new DLLElement(item, sortKey);
    // empty list
    if (pos == NULL)
    {
        first = elem;
        last = elem;
        lock->Release();
        return;
    }
    // *, pos
    if (sortKey <= pos->key)
    {
        elem->next = first;
        first->prev = elem;
        first = elem;
        lock->Release();
        return;
    }
    if (testnum == 6 && tid == 0)
    {
        printf("Thread is preempted\n");
        currentThread->Yield();
    }
    // pos, *, pos->next
    while (pos->next != NULL)
    {
        if (pos->key <= sortKey and pos->next->key >= sortKey)
        {
            elem->prev = pos;
            elem->next = pos->next;
            pos->next->prev = elem;
            pos->next = elem;
            lock->Release();
            return;
        }
        pos = pos->next;
    }
    // pos, *
    elem->prev = last;
    last->next = elem;
    last = elem;
    lock->Release();
    return;
}

void *DLList::SortedRemove(int sortKey, int testnum, int tid)
{
    lock->Acquire();
    DLLElement *elem = first;
    while (elem != NULL)
    {
        if (elem->key == sortKey)
        {
            void *item = elem->item;
            if (elem->prev != NULL)
                elem->prev->next = elem->next;
            else
                first = elem->next;
            if (elem->next != NULL)
                elem->next->prev = elem->prev;
            else
                last = elem->prev;
            printf("threads %d remove point %d successfully\n", tid, elem->key);
            if (testnum == 5 && tid == 1)
            {
                elem->next = NULL; //直接删除会出现段错误，为了演示，暂时把它置为空
                elem->prev = NULL;
            }
            else
                delete elem;
            lock->Release();
            return item;
        }
        if (testnum == 5 && tid == 0 && elem->key == 2)
        {
            printf("Thread is preempted\n");
            currentThread->Yield();
        }
        elem = elem->next;
    }
    lock->Release();
    return NULL;
}
