#include "synth/node_builder.h"

#include <sstream>

namespace analog3 {

NodeBuilder::Prop NodeBuilder::Prop::NODE_TYPE("node_type", DataType::STRING);
NodeBuilder::Prop NodeBuilder::Prop::NODE_NAME("node_name", DataType::STRING);
NodeBuilder::Prop NodeBuilder::Prop::MODEL_NAME("model_name", DataType::STRING);
NodeBuilder::Prop NodeBuilder::Prop::MODEL_ID("model_id", DataType::INT);
NodeBuilder::Prop NodeBuilder::Prop::CHANNEL("channel", DataType::STRING);
NodeBuilder::Prop NodeBuilder::Prop::CHILD_NODES("child_nodes", DataType::ARRAY);
NodeBuilder::Prop NodeBuilder::Prop::OPTIONS("options", DataType::ARRAY);

SynthNode* NodeBuilder::Build() {
  if (!_doc->IsObject()) {
    throw AppError(Status::SCHEMA_NOT_AN_OBJECT);
  }
  //  return BuildNode(&(*_doc)[""]);
  return BuildNode(*_doc);
}

std::string NodeBuilder::GetString(const rapidjson::Value& value, const NodeBuilder::Prop& prop, bool is_required) {
  if (prop.type() != DataType::STRING) {
    std::stringstream ss;
    ss << prop.key() << " should be a string";
    throw AppError(Status::SCHEMA_PROPERTY_MISUSE, ss.str());
  }
  return CheckProp(value, prop, is_required) ? value[prop.key()].GetString() : "";
}

int NodeBuilder::GetInt(const rapidjson::Value& value, const NodeBuilder::Prop& prop, bool is_required) {
  if (prop.type() != DataType::INT) {
    std::stringstream ss;
    ss << prop.key() << " should be an integer";
    throw AppError(Status::SCHEMA_PROPERTY_MISUSE, ss.str());
  }
  return CheckProp(value, prop, is_required) ? value[prop.key()].GetInt() : 0;
}

bool NodeBuilder::CheckProp(const rapidjson::Value& value, const NodeBuilder::Prop& prop, bool is_required) {
  if (!value.HasMember(prop.key())) {
    if (is_required) {
      throw AppError(Status::SCHEMA_REQUIRED_PROPERTY_MISSING, prop.key());
    } else {
      return false;
    }
  }
  bool is_valid_type;
  const char *expected_type;
  switch (prop.type()) {
    case DataType::STRING:
      is_valid_type = value[prop.key()].IsString();
      expected_type = "string";
      break;
    case DataType::INT:
      is_valid_type = value[prop.key()].IsInt();
      expected_type = "int";
      break;
    case DataType::ARRAY:
      is_valid_type = value[prop.key()].IsArray();
      expected_type = "array";
      break;
    default:
      is_valid_type = false;
      expected_type = "unknown";
  }
  if (!is_valid_type) {
    std::stringstream ss;
    ss << "prop=" << prop.key() << " expected_type=" << expected_type;
    throw AppError(Status::SCHEMA_INVALID_PROPERTY_TYPE, ss.str());
  }
  return true;
}

SynthNode* NodeBuilder::BuildNode(const rapidjson::Value& value) {
  if (!value.IsObject()) {
    throw AppError(Status::SCHEMA_NOT_AN_OBJECT);
  }
  std::string node_type = GetString(value, Prop::NODE_TYPE, true);
  SynthNode* node = NULL;
  try {
    if (node_type == "Module") {
      node = BuildModule(value);
    } else if (node_type == "Switch") {
      node = BuildSwitch(value);
    } else {
      throw AppError(Status::SCHEMA_INVALID_NODE_TYPE, "node_type=" + node_type);
    }
    if (value.HasMember(Prop::CHILD_NODES.key())) {
      for (auto& child : value[Prop::CHILD_NODES.key()].GetArray()) {
        node->AddChild(BuildNode(child));
      }
    }
  } catch (const AppError& error) {
    delete node;
    throw error;
  }
  return node;
}

Module* NodeBuilder::BuildModule(const rapidjson::Value& value) {
  if (!value.IsObject()) {
    throw AppError(Status::SCHEMA_NOT_AN_OBJECT);
  }
  Module* module = new Module();
  try {
    module->SetModelName(GetString(value, Prop::MODEL_NAME, true));
    module->SetModelId(GetInt(value, Prop::MODEL_ID, true));
    if (CheckProp(value, Prop::NODE_NAME, false)) module->SetNodeName(GetString(value, Prop::NODE_NAME, false));
  } catch (const AppError& error) {
    delete module;
    throw error;
  }
  return module;
}

Switch* NodeBuilder::BuildSwitch(const rapidjson::Value& value) {
  if (!value.IsObject()) {
    throw AppError(Status::SCHEMA_NOT_AN_OBJECT);
  }
  Switch* node = new Switch();
  try {
    node->SetNodeName(GetString(value, Prop::NODE_NAME, true));
    if (CheckProp(value, Prop::OPTIONS, false)) {
      node->ClearOptions();
      for (auto& option : value[Prop::OPTIONS.key()].GetArray()) {
        node->AddOption(option.GetString());
      }
    }
  } catch (const AppError& error) {
    delete node;
    throw error;
  }
  return node;
}

}  // namespace analog3
