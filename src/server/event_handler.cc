#include "server/event_handler.h"

#include <errno.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

#include "protocol/synthserv.pb.h"
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
  Status status = _server->AddFd(connfd, EPOLLIN, new SessionHandler(connfd));
  if (status != Status::OK) {
    LOG4CPLUS_ERROR(logger,
                    LOG4CPLUS_TEXT("failed to add the listener fd to poller: " << AppError::StrError(status)));
  }
  return status;
}

SessionHandler::SessionHandler(int fd) {
  _instream = new google::protobuf::io::FileInputStream(fd);
  _outstream = new google::protobuf::io::FileOutputStream(fd);
  // _input = new google::protobuf::io::CodedInputStream(_instream);
}

SessionHandler::~SessionHandler() {
  _instream->Close();
  _outstream->Close();
  delete _instream;
  delete _outstream;
}

Status SessionHandler::HandleEvent(const struct epoll_event& epoll_event) {
  fprintf(stderr, "sock=%d event=%d\n", epoll_event.data.fd, epoll_event.events);
  api::SynthServiceMessage* message = new api::SynthServiceMessage();
  // bool result = message->ParseFromZeroCopyStream(_instream);
  // bool result = message->ParseFromCodedStream(_input);
  google::protobuf::io::CodedInputStream input(_instream);
  uint32_t size;
  if (!input.ReadVarint32(&size)) {
    // We assume the peer closed the connection
    std::cout << "Connection to peer has been lost." << std::endl;
    return Status::SESSION_CONNECTION_CLOSED;
  }

  google::protobuf::io::CodedInputStream::Limit limit = input.PushLimit(size);
  if (!message->MergeFromCodedStream(&input)) {
    // TODO(Naoki): do something
  }
  if (!input.ConsumedEntireMessage()) {
    // TODO(Naoki): do something
  }
  input.PopLimit(limit);

  Status status = Status::OK;

  switch (message->op()) {
    case api::SynthServiceMessage::PING: {
      google::protobuf::io::CodedOutputStream output(_outstream);
      std::cout << "PING" << std::endl;
      message->set_op(api::SynthServiceMessage::PING_RESP);
      output.WriteVarint32(message->ByteSize());
      message->SerializeWithCachedSizes(&output);
    }
      _outstream->Flush();
      break;
    case api::SynthServiceMessage::NONE:
      // Client never sends operation NONE. If the message has this op, it means peer connection was closed.
      std::cout << "Connection closed." << std::endl;
      status = Status::SESSION_CONNECTION_CLOSED;
      break;
    default:
      std::cerr << "NOT YET IMPLEMENTED" << std::endl;
  }

  delete message;

  return status;
}

}  // namespace analog3
