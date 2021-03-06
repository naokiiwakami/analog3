#ifndef SRC_API_SYNTH_NODE_H_
#define SRC_API_SYNTH_NODE_H_

#include <string>
#include <vector>

#include "api/synthserv.pb.h"
#include "server/errors.h"

namespace analog3 {
namespace models {

enum class NodeType: int {
  UNKNOWN = 0,
  MODULE = 1,
  KNOB = 2,
  SWITCH = 3,
  VIRTUAL_INPUT = 4,
  VIRTUAL_OUTPUT = 5,
  PHYSICAL_INPUT = 6,
  PHYSICAL_OUTPUT = 7
};

enum class ChannelType {
  NONE = 0,
  CAN = 1
};

enum class SwitchType: int {
  NONE = 0,
  TOGGLE = 1,
  ROTARY = 2,
  SELECTOR = 3,
  MOMENTARY = 4
};

class SynthNode {
 public:
  explicit SynthNode(NodeType t);
  virtual ~SynthNode();

  /**
   * Decode to a synth node object from SynthNode API message.
   */
  static SynthNode* Decode(const api::SynthNode& src, SynthNode* parent = nullptr);

  /**
   * Encode a SynthNode object to API message.
   */
  void Encode(api::SynthNode* dst) const;

  NodeType GetNodeType() { return _node_type; }

  const std::string& GetNodeName() const { return _node_name; }
  void SetNodeName(const std::string& node_name) { _node_name = node_name; }

  uint8_t GetComponentNumber() const { return _component_number; }
  void SetComponentNumber(uint8_t component_number) { _component_number = component_number; }

  ChannelType GetChannelType() const { return _channel_type; }
  void SetChannelType(enum ChannelType channel_type) { _channel_type = channel_type; }

  uint16_t GetInstanceId() const { return _instance_id; }
  void SetInstanceId(uint16_t id) { _instance_id = id; }

  uint16_t GetInitialValue() const { return _initial_value; }
  void SetInitialValue(uint16_t value) { _initial_value = value; }

  uint16_t GetValue() const { return _value; }
  void SetValue(uint16_t value) { _value = value; }

  virtual bool Validate() /*throw(InvalidNodeException*)*/;

  const std::vector<SynthNode*>& GetChildren() const { return _child_nodes; }
  void AddChild(SynthNode* child_node);
  bool RemoveChild(const std::string& name);
  bool RemoveChild(SynthNode* child_node);
  void DetachFromParent();

  SynthNode* GetParent() { return _parent; }

 protected:
  // metadata
  NodeType _node_type;
  std::string _node_name;
  uint8_t _component_number;
  ChannelType _channel_type;

  uint16_t _instance_id;
  uint16_t _initial_value;

  // variables
  uint16_t _value;

  SynthNode* _parent;
  std::vector<SynthNode*> _child_nodes;
};

class Knob : public SynthNode {
 public:
  Knob()
      : SynthNode(NodeType::KNOB), _min_value(0), _max_value(65535), _offset(0), _scale(1)
  {}

  virtual ~Knob() {}

  uint16_t GetMinValue() const { return _min_value; }
  void SetMinValue(uint16_t value) { _min_value = value; }

  uint16_t GetMaxValue() const { return _max_value; }
  void SetMaxValue(uint16_t value) { _max_value = value; }

  uint16_t GetOffset() const { return _offset; }
  void SetOffset(uint16_t offset) { _offset = offset; }

  uint16_t GetScale() const { return _scale; }
  void SetScale(uint16_t scale) { _scale = scale; }

 private:
  uint16_t _min_value;
  uint16_t _max_value;
  uint16_t _offset;
  uint16_t _scale;
};

class Switch : public SynthNode {
 public:
  Switch() : SynthNode(NodeType::SWITCH) {
    _switch_type = SwitchType::NONE;
    _options.push_back("off");
    _options.push_back("on");
  }
  virtual ~Switch() {}

  SwitchType GetSwitchType() const { return _switch_type; }
  void SetSwitchType(SwitchType switch_type) { _switch_type = switch_type; }

  const std::vector<std::string>& GetOptions() const { return _options; }
  void SetOptions(const std::vector<std::string>& options) { _options = options; }
  void AddOption(const std::string& option) {
    _options.push_back(option);
  }
  void ClearOptions() {
    _options.clear();
  }

 private:
  SwitchType _switch_type;
  std::vector<std::string> _options;
};

}  // namespace models
}  // namespace analog3
#endif  // SRC_API_SYNTH_NODE_H_
