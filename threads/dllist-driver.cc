#include "dllist.h"
#include "system.h"

void CreateN(DLList *list, int n, int keys[])
{
    for (int i = 0; i < n; i++)
    {
        keys[i] = (int)rand() % 1024;
        list->SortedInsert(NULL, keys[i], 0, 0);
    }
}

void RemoveN(DLList *list, int n)
{
    int count = 0;
    printf("removed keys order:\n");
    for (int i = 0; i < n; i++)
    {
        int key = -1;
        if (list->Remove(&key) != NULL || key != -1)
        {
            printf("removed %d \n", key);
            count++;
        }
    }
    printf("total removed: %d\n", count);
}
