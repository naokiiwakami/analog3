#include "Thread.h"
#include "Runnable.h"

Thread::Thread(Runnable* runnable)
    : m_runnable(runnable)
{
}

Thread::~Thread()
{
    delete m_runnable;
}

void
Thread::launch()
{
    pthread_create(&m_threadId, NULL, start, this);
}

void*
Thread::start(void* arg)
{
    Thread* thread = (Thread*) arg;
    thread->m_runnable->run();
    delete thread;

    return NULL;
}
