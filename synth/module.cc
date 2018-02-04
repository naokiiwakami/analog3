#include "synth/module.h"

namespace analog3 {

Module::Module()
    : SynthNode(NodeType::kNodeTypeModule), model_id(0) {
}

Module::~Module() {
}

bool Module::Validate() {
  return SynthNode::Validate() && !model_name.empty() && model_id != 0;
}

}  // namespace analog3
