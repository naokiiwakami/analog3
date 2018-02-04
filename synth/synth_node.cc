#include "synth/synth_node.h"

namespace analog3 {

SynthNode::SynthNode(NodeType t)
    : node_type(t), instance_id(0) {
}

SynthNode::~SynthNode() {
  for (auto node : child_nodes) {
    delete node;
  }
}

bool SynthNode::Validate() {
  return !node_name.empty() && instance_id != 0;
}

void SynthNode::AddChild(SynthNode* child_node) {
  child_nodes.push_back(child_node);
}

bool SynthNode::RemoveChild(const std::string& name) {
  for (std::list<SynthNode*>::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it) {
    if ((*it)->node_name == name) {
      child_nodes.erase(it);
      return true;
    }
  }
  return false;
}

bool SynthNode::RemoveChild(SynthNode* child_node) {
  for (std::list<SynthNode*>::iterator it = child_nodes.begin(); it != child_nodes.end(); ++it) {
    if (*it == child_node) {
      child_nodes.erase(it);
      return true;
    }
  }
  return false;
}

void SynthNode::DetachFromParent() {
  parent->RemoveChild(this);
}

}  // namespace analog3
