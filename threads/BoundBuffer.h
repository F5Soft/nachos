#include "synch.h"

class BoundedBuffer
{
public:
    BoundedBuffer(int maxsize);
    ~BoundedBuffer();
    void Read(void *data, int size);
    void Write(void *data, int size);

private:
    char *buffer;
    int maxsize, in, out;

    Semaphore *sMutex;
    Semaphore *sRead;
    Semaphore *sWrite;
    Semaphore *sAvailable;
    Semaphore *sRemaining;
};
