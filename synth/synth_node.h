#ifndef SYNTH_SYNTH_NODE_H_
#define SYNTH_SYNTH_NODE_H_

#include <list>
#include <string>
#include <vector>
#include "synth/errors.h"

namespace analog3 {

enum class NodeType {
  kNodeTypeRig,
  kNodeTypeModule,
  kNodeTypeKnob,
  kNodeTypeSwitch,
  kNodeTypeSelect,
  kNodeTypeVirtualInput,
  kNodeTypeVirtualOutput,
  kNodeTypePhysicalInput,
  kNodeTypePhysicalOutput,
};

class SynthNode {
 public:
  explicit SynthNode(NodeType t);
  virtual ~SynthNode();

  void SetNodeName(const std::string& node_name) { this->node_name = node_name; }
  std::string GetNodeName() { return node_name; }
  enum NodeType GetNodeType() { return node_type; }
  uint16_t GetModelId() { return model_id; }
  uint16_t GetInstanceId() { return instance_id; }

  virtual bool Validate() /*throw(InvalidNodeException*)*/;

  void AddChild(SynthNode* child_node);
  bool RemoveChild(const std::string& name);
  bool RemoveChild(SynthNode* child_node);
  void DetachFromParent();

 protected:
  NodeType node_type;
  uint16_t model_id;
  std::string node_name;
  uint16_t instance_id;

  SynthNode* parent;
  std::list<SynthNode*> child_nodes;
};

class Switch : public SynthNode {
 public:
  Switch() : SynthNode(NodeType::kNodeTypeSelect) {
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

  const std::vector<std::string>& Getptions() {
    return _options;
  }

 private:
  std::vector<std::string> _options;
};

}  // namespace analog3
#endif  // SYNTH_SYNTH_NODE_H_
