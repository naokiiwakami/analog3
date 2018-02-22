#ifndef SRC_SERVER_SYNTH_H_
#define SRC_SERVER_SYNTH_H_

#include <string>
#include <unordered_map>
#include "server/module.h"

namespace analog3 {

class Synth {
 public:
  Synth();
  virtual ~Synth();

  Status AddModelEntry(Module* module);

 private:
  std::unordered_map<int, Module*> models;
};

}  // namespace analog3

#endif  // SRC_SERVER_SYNTH_H_
