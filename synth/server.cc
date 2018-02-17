#include "synth/server.h"

#include <errno.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <boost/circular_buffer.hpp>

#include "synth/event_handler.h"

/*
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
*/

// #include "common.hxx"
// #include "Session.h"
// #include "SessionFactory.h"
// #include "Thread.h"

namespace analog3 {

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Server"));

Server::Server(uint16_t port)
    : _listener_port(port),
      _listener_fd(-1),
      _listener_backlog(10),
      _tid(0),
      _finish_status(Status::OK)
{}

Server::~Server() {
  if (_listener_fd != -1) {
    close(_listener_fd);
  }
}

Status Server::Initialize() {
  struct sockaddr_in myaddr = {};
  myaddr.sin_family      = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port        = htons(_listener_port);

  // Create a server socket
  if ((_listener_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("socket open failure: " << strerror(errno)));
    return Status::SERVER_INIT_FAILED;
  }

  // Set socket options
  int flag = 1;
  if (setsockopt(_listener_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &flag, sizeof(flag)) < 0 ||
      setsockopt(_listener_fd, SOL_SOCKET, SO_KEEPALIVE, (const char *) &flag, sizeof(flag)) < 0) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("setsockopt failure: " << strerror(errno)));
    return Status::SERVER_INIT_FAILED;
  }

  // Bind
  if (bind(_listener_fd, reinterpret_cast<struct sockaddr*>(&myaddr), sizeof(myaddr)) < 0) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("bind failure: " << strerror(errno)));
    return Status::SERVER_INIT_FAILED;
  }

  // Listen
  if (listen(_listener_fd, _listener_backlog) < 0) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("listn failure: " << strerror(errno)));
    return Status::SERVER_INIT_FAILED;
  }

  // Create epoll fd
  _epoll_fd = epoll_create1(0);
  if (_epoll_fd == -1) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("epoll_create failure: " << strerror(errno)));
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
  status = server->Run();
  return nullptr;
}

Status Server::AddFd(int fd, uint32_t events, EventHandler* handler) {
  _fd_table[fd] = handler;
  struct epoll_event event;
  event.events = events;
  event.data.fd = fd;
  if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("epoll_ctrl add failure: " << strerror(errno)));
    return Status::SERVER_SCHEDULER_ERROR;
  }
  return Status::OK;
}

Status Server::DelFd(int fd) {
  if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("epoll_ctrl del failure: " << strerror(errno)));
    return Status::SERVER_SCHEDULER_ERROR;
  }
  _fd_table.erase(fd);
  return Status::OK;
}

Status Server::Run() {
  Status status = AddFd(_listener_fd, EPOLLIN, new AcceptHandler(this));
  if (status != Status::OK) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("failed to add the listener fd to poller: " << AppError::StrError(status)));
    return status;
  }

  const int kMaxEvents = 64;
  struct epoll_event epoll_events[kMaxEvents];
  boost::circular_buffer<Event> events(kMaxEvents);

  while (true) {
    int num_events = epoll_wait(_epoll_fd, epoll_events, kMaxEvents - events.size(), -1);
    if (num_events == -1) {
      LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("poller wait error: " << strerror(errno)));
      return Status::SERVER_SCHEDULER_ERROR;
    }
    for (int i = 0; i < num_events; ++i) {
      events.push_back(Event(_fd_table[epoll_events[i].data.fd], epoll_events[i]));
    }
    while (!events.empty()) {
      status = Status::OK;
      const Event& event = events.front();
      uint32_t revents = event.epoll_event.events;
      if ((revents & (EPOLLIN | EPOLLOUT)) == 0 ||
          (status = event.handler->HandleEvent(event.epoll_event)) != Status::OK) {
        // close socket on detecting an unexpected event or handler status
        if (status != Status::OK) {
          if (status == Status::SESSION_CONNECTION_CLOSED) {
            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("session terminated"));
          } else {
            LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT("event handler returns an error, closing socket"));
          }
        } else {
          LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("poll detects an unexpected event: " << revents));
        }
        DelFd(event.epoll_event.data.fd);
        close(event.epoll_event.data.fd);
      }
      events.pop_front();
    }
  }
  return Status::OK;
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
