#ifndef SRC_SERVER_NODE_BUILDER_H_
#define SRC_SERVER_NODE_BUILDER_H_

#include <string>

#include "rapidjson/document.h"
#include "api/module.h"
#include "api/synth_node.h"

namespace analog3 {

class NodeBuilder {
 public:
  explicit NodeBuilder(rapidjson::Document *doc)
      : _doc(doc)
  {}

  models::SynthNode* Build();

  enum class DataType {
    STRING,
    INT,
    UINT16,
    ARRAY,
  };

  class Prop;

 private:
  std::string GetString(const rapidjson::Value& value, const Prop& prop, bool is_required);

  int GetInt(const rapidjson::Value& value, const Prop& prop, bool is_required);

  uint16_t GetUint16(const rapidjson::Value& value, const Prop& prop, bool is_required);

  bool CheckProp(const rapidjson::Value& value, const Prop& prop, bool is_required);

  models::SynthNode* BuildNode(const rapidjson::Value& value);

  models::Module* BuildModule(const rapidjson::Value& value);

  models::Knob* BuildKnob(const rapidjson::Value& value);

  models::Switch* BuildSwitch(const rapidjson::Value& value);

  rapidjson::Document* _doc;
};

class NodeBuilder::Prop {
 public:
  static Prop NODE_TYPE;
  static Prop NODE_NAME;
  static Prop MODEL_NAME;
  static Prop MODEL_ID;
  static Prop INITIAL_VALUE;
  static Prop MIN_VALUE;
  static Prop MAX_VALUE;
  static Prop CHANNEL;
  static Prop CHILD_NODES;
  static Prop OPTIONS;

  Prop(const char *k, NodeBuilder::DataType t)
      : _key(k), _type(t)
  {}

  const char* key() const { return _key; }
  NodeBuilder::DataType type() const { return _type; }

 private:
  const char *_key;
  NodeBuilder::DataType _type;
};

}  // namespace analog3
#endif  // SRC_SERVER_NODE_BUILDER_H_
