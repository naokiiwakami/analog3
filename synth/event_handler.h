#ifndef SYNTH_EVENT_HANDLER_H_
#define SYNTH_EVENT_HANDLER_H_

#include "synth/errors.h"

namespace analog3 {

/**
 * Abstract class to handle I/O events
 */
class EventHandler {
 public:
  /**
   * ctor
   * @param fd File descriptor where the handler receives events.
   */
  explicit EventHandler(int fd)
      : _fd(fd)
  {}

  virtual ~EventHandler() {}

  /**
   * This method is called when epoll receives an event at corresponding file descriptor.
   * @param events Epoll events
   * @return Status::OK, Status::SERVER_HANDLER_TERM, or an error code
   */
  virtual Status HandleEvent(uint32_t events) = 0;

  inline int GetFd() const { return _fd; }

 private:
  int _fd;
};

}  // namespace analog3
#endif  // SYNTH_EVENT_HANDLER_H_
