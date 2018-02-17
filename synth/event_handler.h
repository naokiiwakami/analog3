#ifndef SYNTH_EVENT_HANDLER_H_
#define SYNTH_EVENT_HANDLER_H_

#include <sys/epoll.h>

#include "synth/errors.h"

namespace analog3 {

class Server;
class Event;

/**
 * Abstract class to handle I/O events
 */
class EventHandler {
 public:
  /**
   * ctor
   * @param fd File descriptor where the handler receives events.
   */
  EventHandler() {}

  virtual ~EventHandler() {}

  /**
   * This method is called when epoll receives an event at corresponding file descriptor.
   * @param events Epoll events
   * @return Status::OK, Status::SERVER_HANDLER_TERM, or an error code
   */
  virtual Status HandleEvent(const struct epoll_event& epoll_event) = 0;
};

struct Event {
  EventHandler* handler;
  struct epoll_event epoll_event;
  Event(EventHandler* h, const struct epoll_event& e)
      : handler(h), epoll_event(e)
  {}
};

class AcceptHandler : public EventHandler {
 public:
  explicit AcceptHandler(Server* server)
      : _server(server)
  {}

  ~AcceptHandler() {}

  Status HandleEvent(const struct epoll_event& epoll_event);

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
  Status HandleEvent(const struct epoll_event& epoll_event);
};

}  // namespace analog3
#endif  // SYNTH_EVENT_HANDLER_H_
