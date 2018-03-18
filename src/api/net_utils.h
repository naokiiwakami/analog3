#ifndef SRC_API_NET_UTILS_H_
#define SRC_API_NET_UTILS_H_

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/message_lite.h>

namespace analog3 {
namespace api {

class NetUtils {
 public:
  static int WriteToStream(const google::protobuf::MessageLite& message,
                           google::protobuf::io::ZeroCopyOutputStream* outstream);

  static int ReadFromStream(google::protobuf::io::ZeroCopyInputStream* instream,
                            google::protobuf::MessageLite* message);
};

}  // namespace api
}  // namespace analog3

#endif  // SRC_API_NET_UTILS_H_
