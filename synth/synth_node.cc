#include "synth/synth_node.h"

namespace analog3 {

SynthNode::SynthNode(NodeType t)
    : _node_type(t), _instance_id(0), _initial_value(0), _value(0), _parent(NULL) {
}

SynthNode::~SynthNode() {
  for (auto node : _child_nodes) {
    delete node;
  }
}

bool SynthNode::Validate() {
  return !_node_name.empty() && _instance_id != 0;
}

void SynthNode::AddChild(SynthNode* child_node) {
  _child_nodes.push_back(child_node);
}

bool SynthNode::RemoveChild(const std::string& name) {
  for (std::list<SynthNode*>::iterator it = _child_nodes.begin(); it != _child_nodes.end(); ++it) {
    if ((*it)->_node_name == name) {
      _child_nodes.erase(it);
      return true;
    }
  }
  return false;
}

bool SynthNode::RemoveChild(SynthNode* child_node) {
  for (std::list<SynthNode*>::iterator it = _child_nodes.begin(); it != _child_nodes.end(); ++it) {
    if (*it == child_node) {
      _child_nodes.erase(it);
      return true;
    }
  }
  return false;
}

void SynthNode::DetachFromParent() {
  _parent->RemoveChild(this);
}

}  // namespace analog3
