#ifndef SRC_MANAGER_APP_H_
#define SRC_MANAGER_APP_H_

#include <string>
#include <vector>

#include "manager/synth_service.h"

namespace analog3 {

class App {
 public:
  explicit App(bool is_stub);
  virtual ~App();

  int run();

 private:
  bool ProcessInput(const std::vector<std::string>& args);

  SynthService *_service;

  static const char* prompt;
};

}  // namespace analog3
#endif  // SRC_MANAGER_APP_H_
