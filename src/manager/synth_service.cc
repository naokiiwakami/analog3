#include "manager/synth_service.h"
#include <iostream>

namespace analog3 {

const uint64_t SynthService::DEFAULT_SYNC_TIMEOUT = 10000;  // 10s

SynthService::SynthService()
    : _next_sequence_num(0) {
  _sync.timeout = DEFAULT_SYNC_TIMEOUT;
  pthread_mutex_init(&_sync.lock, nullptr);
  pthread_cond_init(&_sync.cond, nullptr);
  _arena = new google::protobuf::Arena();
}

StubSynthService::StubSynthService()
    : SynthService() {
}

void StubSynthService::Ping() {
  a3proto::SynthServiceMessage *request = google::protobuf::Arena::CreateMessage<a3proto::SynthServiceMessage>(_arena);
  request->set_op(a3proto::SynthServiceMessage::PING);
  Call(request, [](a3proto::SynthServiceMessage *resp) {
      std::cout << "PONG" << std::endl;
    });
}

void StubSynthService::Call(a3proto::SynthServiceMessage *request,
                            void (*cb)(a3proto::SynthServiceMessage *resp)) {
  request->set_sequence_num(_next_sequence_num++);
  a3proto::SynthServiceMessage *resp = google::protobuf::Arena::CreateMessage<a3proto::SynthServiceMessage>(_arena);
  cb(resp);
}

}  // namespace analog3
