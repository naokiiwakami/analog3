#ifndef _thread_hxx_
#define _thread_hxx_

#include <pthread.h>

class Runnable;

class Thread {
public:
    Thread(Runnable* runnable);
    virtual ~Thread();

    void launch();

protected:
    Runnable* m_runnable;
    pthread_t m_threadId;

private:
    Thread();

    static void *start(void* arg);
};

#endif
