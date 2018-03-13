#include "api/synth_service.h"
#include <error.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "api/net_utils.h"

namespace analog3 {
namespace api {

const uint64_t SynthService::DEFAULT_TIMEOUT = 10000;  // 10s

SynthService::SynthService()
    : _next_sequence_num(0) {
  _arena = new google::protobuf::Arena();
  _shutdown = false;
}

SynthService::~SynthService() {
  delete _arena;
}

int SynthService::Plugin() {
  int status = Start();
  if (status != 0) {
    return status;
  }
  pthread_create(&_pt, nullptr, ReceptorMain, this);
  return 0;
}

int SynthService::Plugout() {
  _shutdown = true;
  Notify();
  pthread_join(_pt, nullptr);
  return Stop();
}

class SyncAcceptor : public Consumer<api::SynthServiceMessage*> {
 public:
  SyncAcceptor() {
    pthread_mutex_init(&_lock, nullptr);
    pthread_cond_init(&_cond, nullptr);
  }

  virtual ~SyncAcceptor() {
    pthread_mutex_destroy(&_lock);
    pthread_cond_destroy(&_cond);
  }

  void Accept(api::SynthServiceMessage* response) {
    HandleMessage(response);
    pthread_mutex_lock(&_lock);
    pthread_cond_signal(&_cond);
    pthread_mutex_unlock(&_lock);
  }

  void Wait() {
    pthread_mutex_lock(&_lock);
    pthread_cond_wait(&_cond, &_lock);
    pthread_mutex_unlock(&_lock);
  }

  virtual void HandleMessage(api::SynthServiceMessage* response) = 0;

 private:
  pthread_mutex_t _lock;
  pthread_cond_t _cond;
};

class PingConsumer : public SyncAcceptor {
 public:
  PingConsumer() : SyncAcceptor() {}
  ~PingConsumer() {}
  void HandleMessage(api::SynthServiceMessage* response) {}
};

class ListModelIdsConsumer : public SyncAcceptor {
 public:
  explicit ListModelIdsConsumer(int32_t* status, std::vector<uint16_t>* model_ids)
      : SyncAcceptor(), _status(status), _model_ids(model_ids) {}
  ~ListModelIdsConsumer() {}

  void HandleMessage(api::SynthServiceMessage* response) {
    *_status = response->status();
    if (*_status == 0) {
      int size = response->model_ids_size();
      for (int i = 0; i < size; ++i) {
        _model_ids->push_back(response->model_ids(i));
      }
    }
  }

 private:
  int32_t* _status;
  std::vector<uint16_t>* _model_ids;
};

// synchronous command processors
void SynthService::Ping() {
  PingConsumer acceptor;
  Request request(_next_sequence_num++, &acceptor);
  api::SynthServiceMessage *message = google::protobuf::Arena::CreateMessage<api::SynthServiceMessage>(_arena);
  message->set_op(api::SynthServiceMessage::PING);
  message->set_sequence_number(request.sequence_number);

  Call(message, &request);

  acceptor.Wait();
}

int SynthService::ListModelIds(std::vector<uint16_t>* model_ids) {
  int32_t status;
  ListModelIdsConsumer acceptor(&status, model_ids);
  Request request(_next_sequence_num++, &acceptor);
  api::SynthServiceMessage *message = google::protobuf::Arena::CreateMessage<api::SynthServiceMessage>(_arena);
  message->set_op(api::SynthServiceMessage::LIST_MODELS);
  message->set_sequence_number(request.sequence_number);

  Call(message, &request);

  acceptor.Wait();
  return status;
}

void SynthService::Call(api::SynthServiceMessage *message, Request* request) {
  in_process[request->sequence_number] = request;
  Send(message);
}

void* SynthService::ReceptorMain(void* arg) {
  SynthService* svc = reinterpret_cast<SynthService*>(arg);
  while (!svc->_shutdown) {
    svc->WaitForEvents();
    while (!svc->received_messages.empty()) {
      svc->HandleReceived(svc->received_messages.front());
      svc->received_messages.pop_front();
    }
  }
  return nullptr;
}

void SynthService::HandleReceived(api::SynthServiceMessage *message_received) {
  auto it = in_process.find(message_received->sequence_number());
  if (it != in_process.end()) {
    in_process.erase(it);
    it->second->acceptor->Accept(message_received);
  }
}

// NetSynthService implementation //////////////////////////////////////////////////////

void HandleSiguser1(int sig) {
  // do nothing
}

NetSynthService::NetSynthService(const std::string& server_host_name, int port)
    : SynthService() {
  _server_host_name = server_host_name;
  _port = port;
  _outstream = nullptr;
  sigset(SIGUSR1, HandleSiguser1);
}

NetSynthService::~NetSynthService() {
  if (_outstream != nullptr) {
    Stop();
  }
}

int NetSynthService::Start() {
  // Create a non-blocking TCP socket.
  _fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if (_fd < 0) {
    // TODO(Naoki): define the status code and eliminate error print
    std::cerr << "Socket create failure: " << strerror(errno) << std::endl;
    return -1;
  }
  // Resolve server host address
  struct hostent* server = gethostbyname(_server_host_name.c_str());
  if (server == nullptr) {
    // TODO(Naoki): define the status code and eliminate error print
    std::cerr << "No such host: " << _server_host_name << std::endl;
    return -1;
  }
  struct sockaddr_in serv_addr = {};
  serv_addr.sin_family = AF_INET;
  bcopy(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(_port);
  // OK ready for connection
  int status = connect(_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  if (status != 0) {
    if (errno == EINPROGRESS) {
      struct pollfd pollfd = {
        .fd = _fd,
        .events = POLLOUT,
        .revents = 0,
      };
      poll(&pollfd, 1, 10000);
      int status = 0;
      unsigned int size = sizeof(int);
      getsockopt(_fd, SOL_SOCKET, SO_ERROR, &status, &size);
      if (status != 0) {
        std::cerr << "Connection failed -- " << strerror(status) << std::endl;
        return -1;
      }
    } else {
      // TODO(Naoki): define the status code and eliminate error print
      std::cerr << "Connection failed: " << strerror(errno) << std::endl;
      return -1;
    }
  }
  _instream = new google::protobuf::io::FileInputStream(_fd);
  _outstream = new google::protobuf::io::FileOutputStream(_fd);
  // _coded_outstream = new google::protobuf::io::CodedOutputStream(_outstream);
  return 0;
}

int NetSynthService::Stop() {
  _outstream->Close();
  _fd = -1;
  delete _outstream;
  _outstream = nullptr;
  // close(_fd);
  return 0;
}

void NetSynthService::Send(api::SynthServiceMessage *message) {
  std::cout << "message info:" << std::endl;
  std::cout << "  IsInitialized = " << message->IsInitialized() << std::endl;
  std::cout << "  ByteSizeLong = " << message->ByteSizeLong() << std::endl;
  std::cout << "  op = " << message->op() << std::endl;
  std::cout << "  seq = " << message->sequence_number() << std::endl;
  std::cout << "  IsMessageTypeSet = " << message->ByteSizeLong() << std::endl;
  std::cout << "... sending" << std::endl;
  api::NetUtils::WriteToStream(*message, _outstream);
  _outstream->Flush();
  std::cout << "...sent" << std::endl;
}

void NetSynthService::Notify() {
  pthread_kill(_pt, SIGUSR1);
}

void NetSynthService::WaitForEvents() {
  struct pollfd pollfd = {
    .fd = _fd,
    .events = POLLIN,
    .revents = 0,
  };
  int result = poll(&pollfd, 1, -1);
  if (result < 0) {
    return;
  }

  // TODO(Naoki): handle connection lost

  api::SynthServiceMessage *message = google::protobuf::Arena::CreateMessage<api::SynthServiceMessage>(_arena);
  api::NetUtils::ReadFromStream(_instream, message);
  received_messages.push_back(message);
}

// StubSynthService implementation //////////////////////////////////////////////////////

StubSynthService::StubSynthService()
    : SynthService() {
  pthread_mutex_init(&_stub_mutex, nullptr);
  pthread_cond_init(&_stub_cond, nullptr);
}

StubSynthService::~StubSynthService() {
}

void StubSynthService::Send(api::SynthServiceMessage *message) {
  api::SynthServiceMessage *response = google::protobuf::Arena::CreateMessage<api::SynthServiceMessage>(_arena);
  switch (message->op()) {
    case api::SynthServiceMessage::PING: {
      response->set_op(api::SynthServiceMessage::PING_RESP);
      response->set_status(0);
      break;
    }
    case api::SynthServiceMessage::LIST_MODELS: {
      response->set_op(api::SynthServiceMessage::LIST_MODELS_RESP);
      response->set_status(0);
      response->add_model_ids(123);
      response->add_model_ids(456);
      break;
    }
    default:
      // TODO(Naoki): handle this
      std::cout << "UNKNOWN" << std::endl;
  }
  response->set_sequence_number(message->sequence_number());
  received_messages.push_back(response);
  Notify();
}

void StubSynthService::Notify() {
  pthread_mutex_lock(&_stub_mutex);
  pthread_cond_signal(&_stub_cond);
  pthread_mutex_unlock(&_stub_mutex);
}

void StubSynthService::WaitForEvents() {
  pthread_mutex_lock(&_stub_mutex);
  pthread_cond_wait(&_stub_cond, &_stub_mutex);
  pthread_mutex_unlock(&_stub_mutex);
}

}  // namespace api
}  // namespace analog3
