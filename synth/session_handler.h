#ifndef SYNTH_SESSION_HANDLER_H_
#define SYNTH_SESSION_HANDLER_H_

#include "synth/event_handler.h"

namespace analog3 {

/**
 * Abstract class to handle I/O events
 */
class SessionHandler : public EventHandler {
 public:
  /**
   * ctor
   * @param fd File descriptor where the handler receives events.
   */
  explicit SessionHandler(int fd)
      : EventHandler(fd)
  {}

  /**
   * This method is called when epoll receives an event at corresponding file descriptor.
   * @param events Epoll events
   * @return Status::OK, Status::SERVER_HANDLER_TERM, or an error code
   */
  Status HandleEvent(uint32_t events);
};

}  // namespace analog3
#endif  // SYNTH_SESSION_HANDLER_H_
