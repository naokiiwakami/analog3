#ifndef SYNTH_SYNTH_H_
#define SYNTH_SYNTH_H_

#include <string>
#include <unordered_map>
#include "synth/module.h"

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

#endif  // SYNTH_SYNTH_H_
