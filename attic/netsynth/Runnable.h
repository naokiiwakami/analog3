#ifndef _runnable_hxx_
#define _runnable_hxx_

class Runnable
{
public:
    virtual ~Runnable() {}
    virtual void run() = 0;
};

#endif
