#ifndef SYNTH_SERVER_H_
#define SYNTH_SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "synth/errors.h"

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

  Status GetFinishStatus() { return _finish_status; }

  // bool waitForShutdown();

  int acceptLoop();

 private:
  void shutdown();

 private:
  uint16_t _listener_port;
  int _listener_fd;
  int _listener_backlog;
  pthread_t _tid;
  Status _finish_status;
};

}  // namespace analog3
#endif  // SYNTH_SERVER_H_
