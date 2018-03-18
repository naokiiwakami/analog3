#ifndef SRC_SERVER_SYNTH_H_
#define SRC_SERVER_SYNTH_H_

#include <string>
#include <unordered_map>
#include "api/module.h"

namespace analog3 {

class Synth {
 public:
  Synth();
  virtual ~Synth();

  Status AddModelEntry(models::Module* module);

 private:
  std::unordered_map<int, models::Module*> models;
};

}  // namespace analog3

#endif  // SRC_SERVER_SYNTH_H_
