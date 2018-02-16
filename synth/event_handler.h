#ifndef SYNTH_EVENT_HANDLER_H_
#define SYNTH_EVENT_HANDLER_H_

#include <poll.h>

#include "synth/errors.h"

namespace analog3 {

class Server;

/**
 * Abstract class to handle I/O events
 */
class EventHandler {
 public:
  /**
   * ctor
   * @param fd File descriptor where the handler receives events.
   */
  EventHandler()
      : _pfd(nullptr)
  {}

  virtual ~EventHandler() {}

  void SetPollFd(struct pollfd* pfd) {
    _pfd = pfd;
  }

  /**
   * This method is called when epoll receives an event at corresponding file descriptor.
   * @param events Epoll events
   * @return Status::OK, Status::SERVER_HANDLER_TERM, or an error code
   */
  virtual Status HandleEvent() = 0;

 protected:
  struct pollfd *_pfd;
};

class AcceptHandler : public EventHandler {
 public:
  explicit AcceptHandler(Server* server)
      : _server(server)
  {}

  ~AcceptHandler() {}

  Status HandleEvent();

 private:
  Server *_server;
};

/**
 * EventHandler subclass that handles a client session
 */
class SessionHandler : public EventHandler {
 public:
  SessionHandler() {}
  ~SessionHandler() {}
  Status HandleEvent();
};

}  // namespace analog3
#endif  // SYNTH_EVENT_HANDLER_H_
