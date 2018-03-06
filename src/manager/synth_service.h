#ifndef SRC_MANAGER_SYNTH_SERVICE_H_
#define SRC_MANAGER_SYNTH_SERVICE_H_

#include <google/protobuf/arena.h>
#include <pthread.h>

#include <atomic>
#include <iostream>

#include "protocol/synthserv.pb.h"

namespace analog3 {

template<typename T>
class Consumer {
 public:
  virtual ~Consumer() {}
  virtual void accept(T t) = 0;
};

class MyConsumer : public Consumer<const char*> {
 public:
  void accept(const char* t) {
    std::cout << t << std::endl;
  }
};

class SynthService {
 public:
  SynthService();
  virtual ~SynthService() {}

  virtual void Ping() = 0;

  static const uint64_t DEFAULT_SYNC_TIMEOUT;  // millisecond

 protected:
  google::protobuf::Arena *_arena;
  std::atomic<uint32_t> _next_sequence_num;
  // synchronization
  struct {
    uint64_t timeout;
    pthread_mutex_t lock;
    pthread_cond_t cond;
  } _sync;
};

class StubSynthService : public SynthService {
 public:
  StubSynthService();
  virtual ~StubSynthService() {}
  void Ping();

 protected:
  void Call(a3proto::SynthServiceMessage *request, void (*cb)(a3proto::SynthServiceMessage *resp));
};

}  // namespace analog3
#endif  // SRC_MANAGER_SYNTH_SERVICE_H_
