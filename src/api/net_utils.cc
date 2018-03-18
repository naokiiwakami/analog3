#include "api/net_utils.h"

#include <google/protobuf/io/coded_stream.h>

#include <iostream>

namespace analog3 {
namespace api {

int NetUtils::WriteToStream(const google::protobuf::MessageLite& message,
                            google::protobuf::io::ZeroCopyOutputStream* outstream) {
  google::protobuf::io::CodedOutputStream output(outstream);
  const int size = message.ByteSize();
  output.WriteVarint32(size);
  uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
  if (buffer != nullptr) {
    message.SerializeWithCachedSizesToArray(buffer);
  } else {
    message.SerializeWithCachedSizes(&output);
    if (output.HadError()) {
      // TODO(Naoki): do something
    }
  }
  return 0;
}

int NetUtils::ReadFromStream(google::protobuf::io::ZeroCopyInputStream* instream,
                             google::protobuf::MessageLite* message) {
  google::protobuf::io::CodedInputStream input(instream);
  uint32_t size;
  if (!input.ReadVarint32(&size)) {
    // std::cout << "Connection to peer has been lost." << std::endl;
    return -1;
  }

  google::protobuf::io::CodedInputStream::Limit limit = input.PushLimit(size);
  if (!message->MergeFromCodedStream(&input)) {
    // TODO(Naoki): do something
    return -1;
  }
  if (!input.ConsumedEntireMessage()) {
    // TODO(Naoki): do something
    return -1;
  }
  input.PopLimit(limit);
  return 0;
}

}  // namespace api
}  // namespace analog3
