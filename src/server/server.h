#ifndef SRC_SERVER_SERVER_H_
#define SRC_SERVER_SERVER_H_

#include <sys/queue.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <boost/container/flat_map.hpp>
#include <functional>
#include <unordered_map>
#include <vector>
#include "server/errors.h"
#include "server/event_handler.h"
#include "server/module.h"

namespace analog3 {

struct ModuleEntry {
  Module* module;
  TAILQ_ENTRY(ModuleEntry) next;

  explicit ModuleEntry(Module* m) {
    module = m;
  }
};

TAILQ_HEAD(ModulesHead, ModuleEntry);

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

  void AddModel(Module* model);

  // void DeleteModel(Module* model);

  Module* GetModel(uint16_t model_id);

  void ForEachModel(std::function<void (Module*)> func);

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

  struct ModulesHead models_head;
  std::unordered_map<uint16_t, ModuleEntry*> models;
};

}  // namespace analog3
#endif  // SRC_SERVER_SERVER_H_
