#include "server/event_handler.h"

#include <errno.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

#include "server/server.h"

namespace analog3 {

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("EventHandler"));

Status AcceptHandler::HandleEvent(const struct epoll_event& epoll_event) {
  struct sockaddr childaddr = {};
  socklen_t len = sizeof(childaddr);
  int connfd;
  if ((connfd = accept4(epoll_event.data.fd, &childaddr, &len, SOCK_NONBLOCK)) == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      // This should not happen, but we log and return to poll anyway
      LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT("accept misses a connection: " << strerror(errno)));
      return Status::OK;
    }
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("accept error: " << strerror(errno)));
    return Status::SERVER_SCHEDULER_ERROR;
  }
  fprintf(stderr, "CONNECT: fd=%d\n", connfd);
  Status status = _server->AddFd(connfd, EPOLLIN, new SessionHandler());
  if (status != Status::OK) {
    LOG4CPLUS_ERROR(logger,
                    LOG4CPLUS_TEXT("failed to add the listener fd to poller: " << AppError::StrError(status)));
  }
  return status;
}

Status SessionHandler::HandleEvent(const struct epoll_event& epoll_event) {
  fprintf(stderr, "sock=%d event=%d\n", epoll_event.data.fd, epoll_event.events);
  char buf[1024];
  ssize_t sz = read(epoll_event.data.fd, buf, sizeof(buf));
  buf[sz] = 0;
  if (sz == 0) {
    fprintf(stderr, "sock=%d connection closed\n", epoll_event.data.fd);
    return Status::SESSION_CONNECTION_CLOSED;
  } else {
    fprintf(stderr, "READ: %s\n", buf);
  }
  return Status::OK;
}

}  // namespace analog3
