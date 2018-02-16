#include "synth/event_handler.h"

#include <errno.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

#include "synth/server.h"

namespace analog3 {

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("EventHandler"));

Status AcceptHandler::HandleEvent() {
  struct sockaddr childaddr = {};
  socklen_t len = sizeof(childaddr);
  int connfd;
  if ((connfd = accept(_pfd->fd, &childaddr, &len)) == -1) {
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("accept error: " << strerror(errno)));
    return Status::SERVER_SCHEDULER_ERROR;
  }
  fprintf(stderr, "CONNECT: fd=%d\n", connfd);
  Status status = _server->AddFd(connfd, POLLIN, new SessionHandler());
  if (status != Status::OK) {
    LOG4CPLUS_ERROR(logger,
                    LOG4CPLUS_TEXT("failed to add the listener fd to poller: " << AppError::StrError(status)));
  }
  return status;
}

Status SessionHandler::HandleEvent() {
  fprintf(stderr, "sock=%d event=%d\n", _pfd->fd, _pfd->revents);
  char buf[1024];
  ssize_t sz = read(_pfd->fd, buf, sizeof(buf));
  buf[sz] = 0;
  if (sz == 0) {
    fprintf(stderr, "sock=%d connection closed\n", _pfd->fd);
    return Status::SESSION_CONNECTION_CLOSED;
  } else {
    fprintf(stderr, "READ: %s\n", buf);
  }
  return Status::OK;
}

}  // namespace analog3
