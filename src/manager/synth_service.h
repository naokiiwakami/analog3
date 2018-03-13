#ifndef SRC_MANAGER_SYNTH_SERVICE_H_
#define SRC_MANAGER_SYNTH_SERVICE_H_

#include <google/protobuf/arena.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <pthread.h>

#include <atomic>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include "api/synthserv.pb.h"

namespace analog3 {

template<typename T>
class Consumer {
 public:
  uint32_t sequence_number;
  virtual ~Consumer() {}
  virtual void Accept(T t) = 0;
};

struct Request {
  uint32_t sequence_number;
  Consumer<api::SynthServiceMessage*>* acceptor;

  Request(uint32_t s, Consumer<api::SynthServiceMessage*>* a) {
    sequence_number = s;
    acceptor = a;
  }
};

/**
 * Synth service connector
 */
class SynthService {
 public:
  SynthService();
  virtual ~SynthService();

  /**
   * Method to connect the service
   */
  int Plugin();

  /**
   * Method to disconnect the service
   */
  int Plugout();

  static const uint64_t DEFAULT_TIMEOUT;  // millisecond

  void Ping();
  int ListModelIds(std::vector<uint16_t>* model_ids);
  // int GetModels(const std::vector<uint16_t>& model_ids, std::vector<Module*> models);

 protected:
  void Call(api::SynthServiceMessage *message, Request* request);

  // Implementation specific methods ///////////////////////////////////////////
  /**
   * Implementation-specific service connection starter.
   *
   * This is called as the first step in Plugin().
   */
  virtual int Start() { return 0; }
  /**
   * Implementation-specific service connection terminator.
   *
   * This is called as the last stop in Plugoff().
   */
  virtual int Stop() { return 0; }
  virtual void Send(api::SynthServiceMessage *message_request) = 0;
  virtual void WaitForEvents() = 0;
  virtual void Notify() = 0;
  // End implementation specific methods ///////////////////////////////////////

  void HandleReceived(api::SynthServiceMessage *message_received);

  std::list<api::SynthServiceMessage*> received_messages;

  static void* ReceptorMain(void* arg);
  pthread_t _pt;
  bool _shutdown;

  google::protobuf::Arena *_arena;
  std::atomic<uint32_t> _next_sequence_num;

  // TODO(Naoki): This can be faster.
  std::unordered_map<uint32_t, Request*> in_process;
  std::list<Request*> timeout_list;
};

class NetSynthService : public SynthService {
 public:
  NetSynthService(const std::string& server_host_name, int port);
  virtual ~NetSynthService();

  int Start();
  int Stop();

 protected:
  virtual void Send(api::SynthServiceMessage *message_request);
  virtual void WaitForEvents();
  virtual void Notify();

 private:
  std::string _server_host_name;
  int _port;
  int _fd;
  google::protobuf::io::FileInputStream* _instream;
  google::protobuf::io::FileOutputStream* _outstream;
};

class StubSynthService : public SynthService {
 public:
  StubSynthService();
  virtual ~StubSynthService();

 protected:
  virtual void Send(api::SynthServiceMessage *message_request);
  virtual void WaitForEvents();
  virtual void Notify();

 private:
  pthread_mutex_t _stub_mutex;
  pthread_cond_t _stub_cond;
};

}  // namespace analog3
#endif  // SRC_MANAGER_SYNTH_SERVICE_H_
