#include "server/event_handler.h"

#include <errno.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

#include "api/net_utils.h"
#include "api/synthserv.pb.h"
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
  Status status = _server->AddFd(connfd, EPOLLIN, new SessionHandler(_server, connfd));
  if (status != Status::OK) {
    LOG4CPLUS_ERROR(logger,
                    LOG4CPLUS_TEXT("failed to add the listener fd to poller: " << AppError::StrError(status)));
  }
  return status;
}

SessionHandler::SessionHandler(Server* server, int fd)
    : EventHandler(server) {
  _instream = new google::protobuf::io::FileInputStream(fd);
  _outstream = new google::protobuf::io::FileOutputStream(fd);
}

SessionHandler::~SessionHandler() {
  _instream->Close();
  _outstream->Close();
  delete _instream;
  delete _outstream;
}

Status SessionHandler::HandleEvent(const struct epoll_event& epoll_event) {
  fprintf(stderr, "sock=%d event=%d\n", epoll_event.data.fd, epoll_event.events);

  api::SynthServiceMessage* request = new api::SynthServiceMessage();
  if (api::NetUtils::ReadFromStream(_instream, request) < 0) {
    // We assume the peer closed the connection
    std::cout << "Connection to peer has been lost." << std::endl;
    delete request;
    return Status::SESSION_CONNECTION_CLOSED;
  }

  Status status = Status::OK;
  api::SynthServiceMessage* response = new api::SynthServiceMessage();
  response->set_sequence_number(request->sequence_number());

  switch (request->op()) {
    case api::SynthServiceMessage::PING:
      std::cout << "PING" << std::endl;
      response->set_op(api::SynthServiceMessage::PING_RESP);
      api::NetUtils::WriteToStream(*response, _outstream);
      _outstream->Flush();
      break;
    case api::SynthServiceMessage::LIST_MODELS:
      std::cout << "LIST_MODELS" << std::endl;
      response->set_op(api::SynthServiceMessage::LIST_MODELS_RESP);
      _server->ForEachModel([&] (models::Module* module) {
          response->add_model_ids(module->GetModelId());
        });
      api::NetUtils::WriteToStream(*response, _outstream);
      _outstream->Flush();
      break;
    case api::SynthServiceMessage::NOT_SET:
      // Client never sends operation NONE. If the message has this op, it means peer connection was closed.
      std::cout << "Connection closed." << std::endl;
      status = Status::SESSION_CONNECTION_CLOSED;
      break;
    default:
      std::cerr << "NOT YET IMPLEMENTED" << std::endl;
  }

  delete request;
  delete response;

  return status;
}

}  // namespace analog3
