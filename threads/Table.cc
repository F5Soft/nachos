#include "Table.h"
#include "thread.h"
#include "system.h"

/*
    Table构造函数：
    1. 初始化表结构锁tablelock
    2. 初始化表空间
    3. value置0 (slot has not been taken)
*/
Table::Table(int size)
{
    tablesize = size;
    tablelock = new Lock("Tablelock");
    table = new void *[size];
    value = new int[size];
    for (int i = 0; i < size; i++) {
        value[i] = 0;
    }
}

/*
    Table析构函数：
    1. 删除表
    2. 删除锁
*/
Table::~Table()
{
    delete[] table;
    delete tablelock;
}

/*
    Alloc函数: 
    为用户数据分配存储位置并返回存储索引（分配失败则返回-1）
    1. 进入函数后加锁，防止被其它进程/线程打断
    2. 遍历value[]寻找空闲位置，存储数据
    3. 无论成功还是失败，都在返回前释放锁，避免死锁
*/
int Table::Alloc(void *object)
{
    int i;
    tablelock->Acquire();
    printf("thread %p alloc", currentThread);
    for (i = 0; i < tablesize; i++) {
        if (value[i] == 0) {
            table[i] = object;
            value[i] = 1;
            printf("table[%d]:%d.\n", i, (int)object);
            tablelock->Release();
            return i;
        }
    }
    printf("alloc failed\n");
    tablelock->Release();
    return -1;
}

/*
    Release函数：
    释放table中指定索引位置存储空间
    1. 进入函数后加锁，防止被其它进程/线程打断
    2. 找到指定位置，删除其中的数据，并置value为0表示此空间没有数据
*/
void Table::Release(int index)
{
    tablelock->Acquire();
    printf("thread %p release", currentThread);
    if (index >= 0 && index < tablesize) {
        if (value[index] == 1) {
            value[index] = 0;
            table[index] = NULL;
            printf("table[%d] has been released.\n", index);
        } 
        else {
            printf("table[%d] is NULL.\n");
        }
    }
    else {
        printf("index %d out of bounds.\n", index);
    }
    tablelock->Release();
    return;
}

/*
    Get函数：
    获取Table指定索引位置存储的内容
    1. 进入函数后加锁，防止被其它进程/线程打断
    2. 遍历Table寻找对应的index
    3. 无论成功还是失败，都在返回前释放锁，避免死锁
*/
void *Table::Get(int index)
{
    tablelock->Acquire();
    printf("thread %p get", currentThread);
    if (index >= 0 && index < tablesize) {
        if (value[index] == 1) {
            printf("table[%d]:%d\n", index,(int)table[index]);
            tablelock->Release();
            return (void *)table[index];
        }
        else {
            printf("table[%d] is empty.\n", index);
        }
    }
    else {
        printf("index %d out of bounds.\n", index);
    }
    tablelock->Release();
    return NULL;
}
