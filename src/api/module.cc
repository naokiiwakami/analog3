#include "api/module.h"

namespace analog3 {
namespace models {

Module::Module()
    : SynthNode(NodeType::MODULE), model_id(0) {
}

Module::~Module() {
}

bool Module::Validate() {
  return SynthNode::Validate() && !model_name.empty() && model_id != 0;
}

}  // namespace models
}  // namespace analog3
