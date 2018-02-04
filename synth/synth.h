#ifndef SYNTH_SYNTH_H_
#define SYNTH_SYNTH_H_

#include <string>
#include <unordered_map>
#include "synth/synth_node.h"

namespace analog3 {

class Synth {
 public:
  Synth();
  virtual ~Synth();

  Status AddModelEntry(SynthNode* model_entry);

 private:
  std::unordered_map<int, SynthNode*> models;
};

}  // namespace analog3

#endif  // SYNTH_SYNTH_H_
