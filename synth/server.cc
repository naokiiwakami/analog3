#include "synth/server.h"

#include <errno.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <vector>
/*
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
*/

// #include "common.hxx"
// #include "Session.h"
// #include "SessionFactory.h"
// #include "Thread.h"

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Server"));

namespace analog3 {

Server::Server(uint16_t port)
    : _listener_port(port),
      _listener_fd(-1),
      _listener_backlog(10),
      _tid(0),
      _finish_status(Status::OK)
{}

Server::~Server() {
}

Status Server::Initialize() {
  struct sockaddr_in myaddr = {};
  myaddr.sin_family      = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port        = htons(_listener_port);

  // Create a server socket
  if ( (_listener_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("socket open failure: " << strerror(errno)));
    return Status::SERVER_INIT_FAILED;
  }

  int flag = 1;
  if (setsockopt(_listener_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &flag, sizeof(flag)) < 0) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("setsockopt failure: " << strerror(errno)));
    return Status::SERVER_INIT_FAILED;
  }

  // Bind
  if ( bind(_listener_fd, reinterpret_cast<struct sockaddr*>(&myaddr), sizeof(myaddr)) < 0 ) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("bind failure: " << strerror(errno)));
    return Status::SERVER_INIT_FAILED;
  }

  // Listen
  if ( listen(_listener_fd, _listener_backlog) < 0 ) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("listn failure: " << strerror(errno)));
    return Status::SERVER_INIT_FAILED;
  }
  return Status::OK;
}

Status Server::Launch() {
  int err = pthread_create(&_tid, nullptr, Server::ThreadMain, this);
  if (err) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("server launch failed: " << strerror(err)));
    return Status::SERVER_LAUNCH_FAILED;
  }
  pthread_join(_tid, nullptr);
  return GetFinishStatus();
}

void* Server::ThreadMain(void *arg) {
  Server *server = reinterpret_cast<Server*>(arg);
  Status& status = server->_finish_status;
  status = Status::OK;

  const int kMaxEvents = 10;

  int epoll_fd = epoll_create(kMaxEvents);
  if (epoll_fd == -1) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("failed to start poller: " << strerror(errno)));
    status = Status::SERVER_SCHEDULER_ERROR;
    return &status;
  }
  server->_epoll_fd = epoll_fd;

  status = server->AddFd(server->_listener_fd, EPOLLIN);
  if (status != Status::OK) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("failed to add the listener fd to poller: " << strerror(errno)));
    return &status;
  }

  struct epoll_event events[kMaxEvents];

  struct sockaddr childaddr;
  socklen_t len;
  int connfd;

  while (true) {
    fprintf(stderr, "start polling\n");
    int nfds = epoll_wait(epoll_fd, events, kMaxEvents, -1);
    if (nfds == -1) {
      LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("poller wait error: " << strerror(errno)));
      status = Status::SERVER_SCHEDULER_ERROR;
      return &status;
    }
    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == server->_listener_fd) {
        memset(&childaddr, 0, sizeof(childaddr));
        len = sizeof(childaddr);
        if ((connfd = accept(server->_listener_fd, &childaddr, &len)) == -1) {
          LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("accept error: " << strerror(errno)));
          status = Status::SERVER_SCHEDULER_ERROR;
          return &status;
        }
        fprintf(stderr, "CONNECT: fd=%d\n", connfd);
        status = server->AddFd(connfd, EPOLLIN);
        if (status != Status::OK) {
          LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("failed to add the listener fd to poller: " << strerror(errno)));
          return &status;
        }
      } else {
        fprintf(stderr, "sock=%d event=%d\n", events[i].data.fd, events[i].events);
        char buf[1024];
        ssize_t sz = read(events[i].data.fd, buf, sizeof(buf));
        buf[sz] = 0;
        if (sz == 0) {
          fprintf(stderr, "sock=%d connection closed\n", events[i].data.fd);
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
          close(events[i].data.fd);
        } else {
          fprintf(stderr, "READ: %s\n", buf);
        }
      }
    }
  }
  return nullptr;
}

Status Server::AddFd(int fd, uint32_t events) {
  struct epoll_event ev;
  ev.events = events;
  ev.data.fd = fd;
  if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == 0) {
    return Status::OK;
  } else {
    return Status::SERVER_SCHEDULER_ERROR;
  }
}

}  // namespace analog3

// Server::~Server()
// {
//     close(_listener_fd);
// }

// int
// Server::acceptLoop()
// {
//     while ( isAcceptAllowed() ) {
//         Session* session = nullptr;
//         Sock* theSock = accept();

//         if (theSock == nullptr) {
//             return -1;
//         }

//         // session = m_sessionFactory->makeSession(theSock);
//         session = new Session(theSock);
//         if (session != nullptr) {
//             Thread* worker = new Thread(session);
//             worker->launch();
//         }
//         else {
//             // TODO: log
//             delete theSock;
//             theSock = nullptr;
//             return -1;
//         }
//     }

//     return 0;
// }

// Sock* Server::accept()
// {
//     struct sockaddr childaddr;
//     memset(&childaddr, 0, sizeof(childaddr));
//     socklen_t len = 0;

//     int connfd;

//     if ( (connfd = ::accept(_listener_fd, &childaddr, &len)) >= 0 ) {
//         return new Sock(connfd);
//     }
//     else {
//         LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("accept error: " << strerror(errno)));
//         return nullptr;
//     }
// }

// bool Server::launch()
// {
//     return pthread_create( &_tid, nullptr, Server::worker, this ) == 0;
// }

// void* Server::worker(void* arg)
// {
//     Server* server = (Server*) arg;
//     server->acceptLoop();
//     return nullptr;
// }

// bool Server::waitForShutdown()
// {
//     return pthread_join( _tid, nullptr ) == 0;
// }

// #endif
