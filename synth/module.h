#ifndef SYNTH_MODULE_H_
#define SYNTH_MODULE_H_

#include <string>
#include "synth/synth_node.h"

namespace analog3 {

class Module : public SynthNode {
 public:
  Module();
  virtual ~Module();
  void SetModelName(const std::string& model_name) { this->model_name = model_name; }
  std::string GetModelName() { return model_name; }
  void SetModelId(uint16_t model_id) { this->model_id = model_id; }
  uint16_t GetModelId() { return model_id; }

  virtual bool Validate();

 private:
  std::string model_name;
  uint16_t model_id;
};

}  // namespace analog3
#endif  // SYNTH_MODULE_H_
