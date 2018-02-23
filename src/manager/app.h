#ifndef SRC_MANAGER_APP_H_
#define SRC_MANAGER_APP_H_

#include <string>
#include <vector>

namespace analog3 {

class App {
 public:
  App();
  virtual ~App();

  void run();

 private:
  bool ProcessInput(const std::vector<std::string>& args);

  static const char* prompt;
};

}  // namespace analog3
#endif  // SRC_MANAGER_APP_H_
