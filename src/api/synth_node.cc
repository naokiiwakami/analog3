#include "api/synth_node.h"

#include "api/module.h"

namespace analog3 {
namespace models {

SynthNode::SynthNode(NodeType t)
    : _node_type(t), _component_number(0), _channel(ChannelType::NONE),
      _instance_id(0), _initial_value(0), _value(0), _parent(NULL) {
}

SynthNode::~SynthNode() {
  for (auto node : _child_nodes) {
    delete node;
  }
}

SynthNode* SynthNode::Decode(const api::SynthNode& src, SynthNode* parent) {
  SynthNode* node;
  switch (src.node_type()) {
    case api::SynthNode::MODULE: {
      Module* module = new Module();
      if (src.has_module()) {
        module->SetModelId(src.module().model_id());
        module->SetModelName(src.module().model_name());
      }
      node = module;
      break;
    }
    case api::SynthNode::KNOB: {
      Knob* knob = new Knob();
      if (src.has_knob()) {
        knob->SetMinValue(src.knob().min_value());
        knob->SetMaxValue(src.knob().max_value());
        knob->SetOffset(src.knob().offset());
        knob->SetScale(src.knob().scale());
      }
      node = knob;
      break;
    }
    case api::SynthNode::SWITCH: {
      Switch* switch_ = new Switch();
      if (src.has_switch_()) {
        SwitchType switch_type;
        switch (src.switch_().switch_type()) {
          case api::Switch::TOGGLE:
            switch_type = SwitchType::TOGGLE;
            break;
          case api::Switch::ROTARY:
            switch_type = SwitchType::ROTARY;
            break;
          case api::Switch::SELECTOR:
            switch_type = SwitchType::SELECTOR;
            break;
          case api::Switch::MOMENTARY:
            switch_type = SwitchType::MOMENTARY;
            break;
          default:
            switch_type = SwitchType::NONE;
        }
        switch_->SetSwitchType(switch_type);
        if (!src.switch_().options().empty()) {
          switch_->ClearOptions();
          for (auto option : src.switch_().options()) {
            switch_->AddOption(option);
          }
        }
      }
      node = switch_;
      break;
    }
    default:
      node = nullptr;
  }
  if (node != nullptr) {
    node->SetNodeName(src.node_name());
    node->_parent = parent;
    // Channel and instance ID of the top node is given.
    // Otherwise are inherited from the parent
    if (parent == nullptr) {
      enum ChannelType channel;
      switch (src.channel()) {
        case api::SynthNode::CAN:
          channel = ChannelType::CAN;
          break;
        default:
          channel = ChannelType::NONE;
      }
      node->SetChannel(channel);
      node->SetInstanceId(src.instance_id());
    } else {
      node->SetChannel(parent->GetChannel());
      node->SetInstanceId(parent->GetInstanceId());
    }
    node->SetComponentNumber(src.component_number());
    node->SetInitialValue(src.initial_value());
  }
  int size = src.child_nodes_size();
  for (int i = 0; i < size; ++i) {
    node->AddChild(Decode(src.child_nodes(i), node));
  }
  return node;
}

void SynthNode::Encode(api::SynthNode* dst) {
  api::SynthNode_NodeType node_type;
  switch (_node_type) {
    case NodeType::MODULE: {
      Module* module = dynamic_cast<Module*>(this);
      node_type = api::SynthNode::MODULE;
      dst->mutable_module()->set_model_id(module->GetModelId());
      dst->mutable_module()->set_model_name(module->GetModelName());
      break;
    }
    case NodeType::KNOB: {
      Knob* knob = dynamic_cast<Knob*>(this);
      node_type = api::SynthNode::KNOB;
      dst->mutable_knob()->set_min_value(knob->GetMinValue());
      dst->mutable_knob()->set_max_value(knob->GetMaxValue());
      dst->mutable_knob()->set_offset(knob->GetOffset());
      dst->mutable_knob()->set_scale(knob->GetScale());
      break;
    }
    case NodeType::SWITCH: {
      Switch* switch_ = dynamic_cast<Switch*>(this);
      node_type = api::SynthNode::SWITCH;
      api::Switch::SwitchType switch_type;
      switch (switch_->GetSwitchType()) {
        case SwitchType::TOGGLE:
          switch_type = api::Switch::TOGGLE;
          break;
        case SwitchType::ROTARY:
          switch_type = api::Switch::ROTARY;
          break;
        case SwitchType::SELECTOR:
          switch_type = api::Switch::SELECTOR;
          break;
        case SwitchType::MOMENTARY:
          switch_type = api::Switch::MOMENTARY;
          break;
        default:
          switch_type = api::Switch::NOT_SET;
      }
      dst->mutable_switch_()->set_switch_type(switch_type);
      for (const std::string& option : switch_->GetOptions()) {
        dst->mutable_switch_()->add_options(option);
      }
      break;
    }
    default:
      node_type = api::SynthNode::NOT_SET;
  }

  dst->set_node_type(node_type);
  dst->set_node_name(GetNodeName());
  dst->set_component_number(GetComponentNumber());
  switch (GetChannel()) {
    case ChannelType::CAN:
      dst->set_channel(api::SynthNode::CAN);
      break;
    default:
      dst->set_channel(api::SynthNode::NONE);
  }
  dst->set_instance_id(GetInstanceId());
  dst->set_initial_value(GetInitialValue());
}

bool SynthNode::Validate() {
  return !_node_name.empty() && _instance_id != 0;
}

void SynthNode::AddChild(SynthNode* child_node) {
  _child_nodes.push_back(child_node);
}

bool SynthNode::RemoveChild(const std::string& name) {
  for (std::vector<SynthNode*>::iterator it = _child_nodes.begin(); it != _child_nodes.end(); ++it) {
    if ((*it)->_node_name == name) {
      delete *it;
      _child_nodes.erase(it);
      return true;
    }
  }
  return false;
}

bool SynthNode::RemoveChild(SynthNode* child_node) {
  for (std::vector<SynthNode*>::iterator it = _child_nodes.begin(); it != _child_nodes.end(); ++it) {
    if (*it == child_node) {
      delete *it;
      _child_nodes.erase(it);
      return true;
    }
  }
  return false;
}

void SynthNode::DetachFromParent() {
  _parent->RemoveChild(this);
}

}  // namespace models
}  // namespace analog3
