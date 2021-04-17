#include "BoundBuffer.h"

// create a bounded buffer with a limit of 'maxsize' bytes
BoundedBuffer::BoundedBuffer(int maxsize)
{
    buffer = new char[maxsize];
    this->maxsize = maxsize;
    in = 0;
    out = 0;
    sMutex = new Semaphore("sMutex", 1);
    sRead = new Semaphore("sRead", 1);
    sWrite = new Semaphore("sWrite", 1);
    sAvailable = new Semaphore("sAvaliable", 0);
    sRemaining = new Semaphore("sRemaining", maxsize);
}

BoundedBuffer::~BoundedBuffer()
{
    delete sMutex, sRead, sWrite, sAvailable, sRemaining;
}

// read 'size' bytes from the bounded buffer, storing into 'data'.
// ('size' may be greater than 'maxsize')
void BoundedBuffer::Read(void *data, int size)
{
    sRead->P();
    // [begin] only one reader can enter this region at a time
    // to keep the data consistency when a reader is reading multiple blocks
    // while other reader wants to read as well
    for (int i = 0; i < size; i++)
    {
        sAvailable->P();
        sMutex->P();
        // [begin] only one reader or writer can enter this region at a time
        ((char *)data)[i] = buffer[out]; // read one byte at a time
        // [end] only one reader or writer can enter this region at a time
        out = (out + 1) % maxsize;
        sMutex->V();
        sRemaining->V();
    }
    // [end] only one reader can enter this region at a time
    sRead->V();
}

// write 'size' bytes from 'data' into the bounded buffer.
// ('size' may be greater than 'maxsize')
void BoundedBuffer::Write(void *data, int size)
{
    sWrite->P();
    // [begin] only one writer can enter this region at a time
    // to keep the data consistency when a writer is writing multiple blocks
    // while other writer wants to write as well
    for (int i = 0; i < size; i++)
    {
        sRemaining->P();
        sMutex->P();
        // [begin] only one reader or writer can enter this region at a time
        buffer[in] = ((char *)data)[i]; // write one byte at a time
        in = (in + 1) % maxsize;
        // [end] only one reader or writer can enter this region at a time
        sMutex->V();
        sAvailable->V();
    }
    // [end] only one writer can enter this region at a time
    sWrite->V();
}