#ifndef _listener_h_
#define _listener_h_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "Sock.h"

class SessionFactory;

class Listener
{
public:
    Listener(int port, SessionFactory* factory);
    virtual ~Listener();

    bool initialize();
    Sock* accept();
    bool isAcceptAllowed() const { return m_allowAccept; }

    int  getPort() const { return _port; }

    bool waitForShutdown();

    int acceptLoop();

    bool launch();
    static void* worker(void* arg);
    
protected:
    void _shutdown();

protected:
    Listener();

    unsigned short _port;
    int _sockfd;
    int _n_ListenQSize;
    bool m_allowAccept;
    pthread_t _th;

    SessionFactory* m_sessionFactory;
};

#endif
