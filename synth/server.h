#ifndef SYNTH_SERVER_H_
#define SYNTH_SERVER_H_

#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <vector>

#include "synth/errors.h"
#include "synth/event_handler.h"

namespace analog3 {

class Server {
 public:
  explicit Server(uint16_t listener_port);
  virtual ~Server();

  Status Initialize();

  Status Launch();
  static void* ThreadMain(void* arg);

  // Sock* accept();

  int  GetPort() const { return _listener_port; }

  int GetListenerFd() const { return _listener_fd; }

  Status GetFinishStatus() { return _finish_status; }

  // bool waitForShutdown();

  // int acceptLoop();

  Status AddFd(int fd, int16_t events, EventHandler* handler);

  Status ModFd(int fd, int16_t events);

  Status DelFd(int index);

 private:
  Status Run();
  // void shutdown();

 private:
  uint16_t _listener_port;
  int _listener_fd;
  int _listener_backlog;

  std::vector<struct pollfd> _fds;
  std::vector<EventHandler*> _handlers;

  pthread_t _tid;
  Status _finish_status;
};

}  // namespace analog3
#endif  // SYNTH_SERVER_H_
