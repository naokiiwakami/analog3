#ifndef SYNTH_NODE_BUILDER_H_
#define SYNTH_NODE_BUILDER_H_

#include <string>

#include "rapidjson/document.h"
#include "synth/module.h"
#include "synth/synth_node.h"

namespace analog3 {

class NodeBuilder {
 public:
  explicit NodeBuilder(rapidjson::Document *doc)
      : _doc(doc)
  {}

  SynthNode* Build();

  enum class DataType {
    STRING,
    INT,
    ARRAY,
  };

  class Prop;

 private:
  std::string GetString(const rapidjson::Value& value, const Prop& prop, bool is_required);

  int GetInt(const rapidjson::Value& value, const Prop& prop, bool is_required);

  bool CheckProp(const rapidjson::Value& value, const Prop& prop, bool is_required);

  SynthNode* BuildNode(const rapidjson::Value& value);

  Module* BuildModule(const rapidjson::Value& value);

  Switch* BuildSwitch(const rapidjson::Value& value);

  rapidjson::Document* _doc;
};

class NodeBuilder::Prop {
 public:
  static Prop NODE_TYPE;
  static Prop NODE_NAME;
  static Prop MODEL_NAME;
  static Prop MODEL_ID;
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
#endif  // SYNTH_NODE_BUILDER_H_
