#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// #include "common.hxx"
#include "Listener.h"
#include "Session.h"
#include "SessionFactory.h"
#include "Thread.h"

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Listener"));

Listener::Listener(int port, SessionFactory* factory)
    : _port(port),
      _sockfd(0),
      _n_ListenQSize(10),
      m_allowAccept(true),
      _th(0),
      m_sessionFactory(factory)
{
}

bool
Listener::initialize()
{
    struct sockaddr_in myaddr;
    memset( &myaddr, 0, sizeof(myaddr) );
    myaddr.sin_family      = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port        = htons(_port);

    // Create a listener socket
    if ( (_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("socket open failure: " << strerror(errno)));
        return false;
    }

    int f_Flag = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR,
                   (const char *) &f_Flag, sizeof(f_Flag)) < 0) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("setsockopt failure: " << strerror(errno)));
        return false;
    }

    // Bind
    if ( bind(_sockfd, reinterpret_cast<struct sockaddr*>(&myaddr), sizeof(myaddr)) < 0 ) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("bind failure: " << strerror(errno)));
        return false;
    }

    // Listen
    if ( listen(_sockfd, _n_ListenQSize) < 0 ) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("listn failure: " << strerror(errno)));
    }
    return true;
}

Listener::~Listener()
{
    m_allowAccept = false;
    close(_sockfd);
}

int
Listener::acceptLoop()
{
    while ( isAcceptAllowed() ) {
        Session* session = NULL;
        Sock* theSock = accept();

        if (theSock == NULL) {
            return -1;
        }

        // session = m_sessionFactory->makeSession(theSock);
        session = new Session(theSock);
        if (session != NULL) {
            Thread* worker = new Thread(session);
            worker->launch();
        }
        else {
            // TODO: log
            delete theSock;
            theSock = NULL;
            return -1;
        }
    }

    return 0;
}

Sock* Listener::accept()
{
    struct sockaddr childaddr;
    memset(&childaddr, 0, sizeof(childaddr));
    socklen_t len = 0;

    int connfd;

    if ( (connfd = ::accept(_sockfd, &childaddr, &len)) >= 0 ) {
        return new Sock(connfd);
    }
    else {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("accept error: " << strerror(errno)));
        return NULL;
    }
}

bool Listener::launch()
{
    return pthread_create( &_th, NULL, Listener::worker, this ) == 0;
}

void* Listener::worker(void* arg)
{
    Listener* listener = (Listener*) arg;
    listener->acceptLoop();
    return NULL;
}

bool Listener::waitForShutdown()
{
    return pthread_join( _th, NULL ) == 0;
}
