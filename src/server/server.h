#ifndef SRC_SERVER_SERVER_H_
#define SRC_SERVER_SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <boost/container/flat_map.hpp>
#include <vector>
#include "server/errors.h"
#include "server/event_handler.h"

namespace analog3 {

class Server {
 public:
  explicit Server(uint16_t listener_port);
  virtual ~Server();

  Status Initialize();

  Status Launch();
  static void* ThreadMain(void* arg);

  int  GetPort() const { return _listener_port; }

  int GetListenerFd() const { return _listener_fd; }

  Status GetFinishStatus() { return _finish_status; }

  Status AddFd(int fd, uint32_t events, EventHandler* handler);

  Status ModFd(int fd, uint32_t events);

  Status DelFd(int fd);

 private:
  Status Run();

 private:
  uint16_t _listener_port;
  int _listener_fd;
  int _listener_backlog;

  // event polling
  int _epoll_fd;
  boost::container::flat_map<int, EventHandler*> _fd_table;

  pthread_t _tid;
  Status _finish_status;
};

}  // namespace analog3
#endif  // SRC_SERVER_SERVER_H_
