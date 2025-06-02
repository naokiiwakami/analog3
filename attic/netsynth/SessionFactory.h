#ifndef _sessionfactory_hxx_
#define _sessionfactory_hxx_

class SessionFactory
{
public:
    virtual Session* makeSession(Sock* sock) = 0;
};

#endif
