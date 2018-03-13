#ifndef SRC_API_SYNTH_NODE_H_
#define SRC_API_SYNTH_NODE_H_

#include <list>
#include <string>
#include <vector>
#include "server/errors.h"

namespace analog3 {
namespace models {

enum class NodeType {
  kNodeTypeModule,
  kNodeTypeKnob,
  kNodeTypeSwitch,
  kNodeTypeVirtualInput,
  kNodeTypeVirtualOutput,
  kNodeTypePhysicalInput,
  kNodeTypePhysicalOutput,
};

class SynthNode {
 public:
  explicit SynthNode(NodeType t);
  virtual ~SynthNode();

  enum NodeType GetNodeType() { return _node_type; }

  void SetNodeName(const std::string& node_name) { _node_name = node_name; }
  std::string GetNodeName() { return _node_name; }

  void SetInstanceId(uint16_t id) { _instance_id = id; }
  uint16_t GetInstanceId() { return _instance_id; }

  void SetInitialValue(uint16_t value) { _initial_value = value; }
  uint16_t GetInitialValue() { return _initial_value; }

  void SetValue(uint16_t value) { _value = value; }
  uint16_t GetValue() { return _value; }

  virtual bool Validate() /*throw(InvalidNodeException*)*/;

  void AddChild(SynthNode* child_node);
  bool RemoveChild(const std::string& name);
  bool RemoveChild(SynthNode* child_node);
  void DetachFromParent();

 protected:
  // metadata
  NodeType _node_type;
  std::string _node_name;
  uint16_t _instance_id;
  uint16_t _initial_value;

  // variables
  uint16_t _value;

  SynthNode* _parent;
  std::list<SynthNode*> _child_nodes;
};

class Knob : public SynthNode {
 public:
  Knob()
      : SynthNode(NodeType::kNodeTypeKnob), _min_value(0), _max_value(1023)
  {}

  virtual ~Knob() {}

  void SetMinValue(uint16_t value) { _min_value = value; }
  uint16_t GetMinValue() { return _min_value; }

  void SetMaxValue(uint16_t value) { _max_value = value; }
  uint16_t GetMaxValue() { return _max_value; }

 private:
  uint16_t _min_value;
  uint16_t _max_value;
};

class Switch : public SynthNode {
 public:
  Switch() : SynthNode(NodeType::kNodeTypeSwitch) {
    _options.push_back("off");
    _options.push_back("on");
  }
  virtual ~Switch() {}

  void ClearOptions() {
    _options.clear();
  }

  void AddOption(const std::string& option) {
    _options.push_back(option);
  }

  const std::vector<std::string>& GetOptions() {
    return _options;
  }

 private:
  std::vector<std::string> _options;
};

}  // namespace models
}  // namespace analog3
#endif  // SRC_API_SYNTH_NODE_H_
