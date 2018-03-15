#include "gtest/gtest.h"
#include "api/module.h"
#include "api/synth_node.h"
#include "api/synthserv.pb.h"

namespace analog3 {
namespace models {

TEST(SynthNodeTest, Module) {
  Module* module = new Module();
  EXPECT_EQ(module->GetNodeType(), NodeType::MODULE);
  EXPECT_EQ(module->GetInitialValue(), 0);
  EXPECT_EQ(module->GetChannel(), ChannelType::NONE);

  module->SetNodeName("abc");
  EXPECT_EQ(module->GetNodeName(), "abc");

  module->SetComponentNumber(255);
  EXPECT_EQ(module->GetComponentNumber(), 255);

  module->SetChannel(ChannelType::CAN);
  EXPECT_EQ(module->GetChannel(), ChannelType::CAN);

  module->SetInitialValue(65535);
  EXPECT_EQ(module->GetInitialValue(), 65535);

  module->SetValue(65535);
  EXPECT_EQ(module->GetValue(), 65535);

  module->SetModelName("minimoog");
  EXPECT_EQ(module->GetModelName(), "minimoog");

  module->SetModelId(2600);
  EXPECT_EQ(module->GetModelId(), 2600);

  delete module;
}

TEST(SynthNodeTest, Knob) {
  Knob* knob = new Knob();
  EXPECT_EQ(knob->GetNodeType(), NodeType::KNOB);
  EXPECT_EQ(knob->GetMinValue(), 0);
  EXPECT_EQ(knob->GetMaxValue(), 65535);
  EXPECT_EQ(knob->GetOffset(), 0);
  EXPECT_EQ(knob->GetScale(), 1);

  knob->SetMinValue(65535);
  EXPECT_EQ(knob->GetMinValue(), 65535);

  knob->SetMaxValue(720);
  EXPECT_EQ(knob->GetMaxValue(), 720);

  knob->SetOffset(65534);
  EXPECT_EQ(knob->GetOffset(), 65534);

  knob->SetScale(65533);
  EXPECT_EQ(knob->GetScale(), 65533);

  delete knob;
}

TEST(SynthNodeTest, Switch) {
  Switch* switch_ = new Switch();
  EXPECT_EQ(switch_->GetNodeType(), NodeType::SWITCH);
  ASSERT_EQ(switch_->GetOptions().size(), 2);
  ASSERT_EQ(switch_->GetOptions()[0], "off");
  ASSERT_EQ(switch_->GetOptions()[1], "on");
  delete switch_;
}

TEST(SynthNodeTest, BuildFromApiMessageModuleSingle) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::MODULE);
  node_desc->set_node_name("test_module");
  node_desc->set_component_number(2);
  node_desc->set_channel(api::SynthNode::CAN);
  node_desc->mutable_module()->set_model_id(800);
  node_desc->mutable_module()->set_model_name("monomonopolypoly");

  // build
  SynthNode* node = SynthNode::Build(*node_desc);

  // verify
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::MODULE);
  Module* module = dynamic_cast<Module*>(node);
  ASSERT_NE(module, nullptr);
  EXPECT_EQ(module->GetNodeName(), "test_module");
  EXPECT_EQ(module->GetComponentNumber(), 2);
  EXPECT_EQ(module->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(module->GetInitialValue(), 0);
  // verify module specific parameters
  EXPECT_EQ(module->GetModelId(), 800);
  EXPECT_EQ(module->GetModelName(), "monomonopolypoly");

  delete module;
  delete node_desc;
}

TEST(SynthNodeTest, BuildFromApiMessageKnobSingle) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::KNOB);
  node_desc->set_node_name("test_knob");
  node_desc->set_component_number(3);
  node_desc->set_initial_value(32767);
  node_desc->mutable_knob()->set_min_value(10);
  node_desc->mutable_knob()->set_max_value(50000);
  node_desc->mutable_knob()->set_offset(25000);
  node_desc->mutable_knob()->set_scale(1024);

  // build
  SynthNode* node = SynthNode::Build(*node_desc);

  // verify
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::KNOB);
  Knob* knob = dynamic_cast<Knob*>(node);
  ASSERT_NE(knob, nullptr);
  EXPECT_EQ(knob->GetNodeName(), "test_knob");
  EXPECT_EQ(knob->GetComponentNumber(), 3);
  EXPECT_EQ(knob->GetChannel(), ChannelType::NONE);
  EXPECT_EQ(knob->GetInitialValue(), 32767);
  // verify knob specific parameters
  EXPECT_EQ(knob->GetMinValue(), 10);
  EXPECT_EQ(knob->GetMaxValue(), 50000);
  EXPECT_EQ(knob->GetOffset(), 25000);
  EXPECT_EQ(knob->GetScale(), 1024);

  delete knob;
  delete node_desc;
}

TEST(SynthNodeTest, BuildFromApiMessageKnobSingleDefault) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::KNOB);
  node_desc->set_node_name("test_knob_default");

  // build
  SynthNode* node = SynthNode::Build(*node_desc);

  // verify
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::KNOB);
  Knob* knob = dynamic_cast<Knob*>(node);
  ASSERT_NE(knob, nullptr);
  EXPECT_EQ(knob->GetNodeName(), "test_knob_default");
  EXPECT_EQ(knob->GetComponentNumber(), 0);
  EXPECT_EQ(knob->GetChannel(), ChannelType::NONE);
  EXPECT_EQ(knob->GetInitialValue(), 0);
  // verify knob specific parameters
  EXPECT_EQ(knob->GetMinValue(), 0);
  EXPECT_EQ(knob->GetMaxValue(), 65535);
  EXPECT_EQ(knob->GetOffset(), 0);
  EXPECT_EQ(knob->GetScale(), 1);

  delete knob;
  delete node_desc;
}

TEST(SynthNodeTest, BuildFromApiMessageSwitchSingle) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::SWITCH);
  node_desc->set_node_name("test_switch");
  node_desc->set_component_number(4);
  node_desc->set_initial_value(1);
  node_desc->mutable_switch_()->add_options("pulse");
  node_desc->mutable_switch_()->add_options("sawtooth");
  node_desc->mutable_switch_()->add_options("triangular");
  node_desc->mutable_switch_()->set_switch_type(api::Switch::ROTARY);

  // build
  SynthNode* node = SynthNode::Build(*node_desc);

  // verify
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::SWITCH);
  Switch* switch_ = dynamic_cast<Switch*>(node);
  ASSERT_NE(switch_, nullptr);
  EXPECT_EQ(switch_->GetNodeName(), "test_switch");
  EXPECT_EQ(switch_->GetComponentNumber(), 4);
  EXPECT_EQ(switch_->GetInitialValue(), 1);
  // verify sw specific parameters
  EXPECT_EQ(switch_->GetSwitchType(), SwitchType::ROTARY);
  ASSERT_EQ(switch_->GetOptions().size(), 3);
  EXPECT_EQ(switch_->GetOptions()[0], "pulse");
  EXPECT_EQ(switch_->GetOptions()[1], "sawtooth");
  EXPECT_EQ(switch_->GetOptions()[2], "triangular");

  delete switch_;
  delete node_desc;
}

TEST(SynthNodeTest, BuildFromApiMessageSwitchSingleDefault) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::SWITCH);
  node_desc->set_node_name("test_switch_default");

  // build
  SynthNode* node = SynthNode::Build(*node_desc);

  // verify
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::SWITCH);
  Switch* switch_ = dynamic_cast<Switch*>(node);
  ASSERT_NE(switch_, nullptr);
  EXPECT_EQ(switch_->GetNodeName(), "test_switch_default");
  EXPECT_EQ(switch_->GetComponentNumber(), 0);
  EXPECT_EQ(switch_->GetInitialValue(), 0);
  // verify sw specific parameters
  EXPECT_EQ(switch_->GetSwitchType(), SwitchType::NONE);
  EXPECT_EQ(switch_->GetOptions().size(), 2);
  EXPECT_EQ(switch_->GetOptions()[0], "off");
  EXPECT_EQ(switch_->GetOptions()[1], "on");

  delete switch_;
  delete node_desc;
}

TEST(SynthNodeTest, BuildFromApiMessageSwitchType) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::SWITCH);
  node_desc->set_node_name("test_switch_types");

  node_desc->mutable_switch_()->set_switch_type(api::Switch::TOGGLE);
  SynthNode* node = SynthNode::Build(*node_desc);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::SWITCH);
  Switch* switch_ = dynamic_cast<Switch*>(node);
  ASSERT_NE(switch_, nullptr);
  EXPECT_EQ(switch_->GetSwitchType(), SwitchType::TOGGLE);
  delete switch_;

  node_desc->mutable_switch_()->set_switch_type(api::Switch::MOMENTARY);
  node = SynthNode::Build(*node_desc);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::SWITCH);
  switch_ = dynamic_cast<Switch*>(node);
  ASSERT_NE(switch_, nullptr);
  EXPECT_EQ(switch_->GetSwitchType(), SwitchType::MOMENTARY);
  delete switch_;

  node_desc->mutable_switch_()->set_switch_type(api::Switch::SELECTOR);
  node = SynthNode::Build(*node_desc);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::SWITCH);
  switch_ = dynamic_cast<Switch*>(node);
  ASSERT_NE(switch_, nullptr);
  EXPECT_EQ(switch_->GetSwitchType(), SwitchType::SELECTOR);
  delete switch_;

  delete node_desc;
}

TEST(SynthNodeTest, BuildFromApiMessageModuleNested) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::MODULE);
  node_desc->set_channel(api::SynthNode::CAN);
  node_desc->set_instance_id(10);
  node_desc->mutable_module()->set_model_id(921);
  node_desc->mutable_module()->set_model_name("vco");

  api::SynthNode* child = node_desc->add_child_nodes();
  child->set_node_type(api::SynthNode::KNOB);
  child->set_node_name("frequency");
  child->set_component_number(1);
  child->set_initial_value(32768);

  child = node_desc->add_child_nodes();
  child->set_node_type(api::SynthNode::SWITCH);
  child->set_node_name("waveform");
  child->set_component_number(2);
  child->mutable_switch_()->add_options("pulse");
  child->mutable_switch_()->add_options("sawtooth");
  child->mutable_switch_()->add_options("triangular");

  child = node_desc->add_child_nodes();
  child->set_node_type(api::SynthNode::MODULE);
  child->set_node_name("modulation submodule");
  child->set_component_number(3);

  child = child->add_child_nodes();  // sub-sub node
  child->set_node_type(api::SynthNode::KNOB);
  child->set_node_name("depth");
  child->set_component_number(4);

  // build
  SynthNode* node = SynthNode::Build(*node_desc);

  // verify
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::MODULE);
  Module* module = dynamic_cast<Module*>(node);
  ASSERT_NE(module, nullptr);
  EXPECT_EQ(module->GetInstanceId(), 10);
  EXPECT_EQ(module->GetComponentNumber(), 0);
  EXPECT_EQ(module->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(module->GetModelId(), 921);
  EXPECT_EQ(module->GetModelName(), "vco");

  ASSERT_EQ(module->GetChildren().size(), 3);

  Knob* knob = dynamic_cast<Knob*>(module->GetChildren()[0]);
  ASSERT_NE(knob, nullptr);
  ASSERT_EQ(knob->GetParent(), module);
  EXPECT_EQ(knob->GetNodeName(), "frequency");
  EXPECT_EQ(knob->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(knob->GetInstanceId(), 10);
  EXPECT_EQ(knob->GetComponentNumber(), 1);
  EXPECT_EQ(knob->GetInitialValue(), 32768);

  Switch* switch_ = dynamic_cast<Switch*>(module->GetChildren()[1]);
  ASSERT_NE(switch_, nullptr);
  ASSERT_EQ(switch_->GetParent(), module);
  EXPECT_EQ(switch_->GetNodeName(), "waveform");
  EXPECT_EQ(switch_->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(switch_->GetInstanceId(), 10);
  EXPECT_EQ(switch_->GetComponentNumber(), 2);
  ASSERT_EQ(switch_->GetOptions().size(), 3);
  EXPECT_EQ(switch_->GetOptions()[0], "pulse");
  EXPECT_EQ(switch_->GetOptions()[1], "sawtooth");
  EXPECT_EQ(switch_->GetOptions()[2], "triangular");

  Module* sub_module = dynamic_cast<Module*>(module->GetChildren()[2]);
  ASSERT_NE(sub_module, nullptr);
  ASSERT_EQ(sub_module->GetParent(), module);
  EXPECT_EQ(sub_module->GetNodeName(), "modulation submodule");
  EXPECT_EQ(sub_module->GetInstanceId(), 10);
  EXPECT_EQ(sub_module->GetComponentNumber(), 3);
  EXPECT_EQ(sub_module->GetChannel(), ChannelType::CAN);
  ASSERT_EQ(sub_module->GetChildren().size(), 1);

  knob = dynamic_cast<Knob*>(sub_module->GetChildren()[0]);
  ASSERT_NE(knob, nullptr);
  ASSERT_EQ(knob->GetParent(), sub_module);
  EXPECT_EQ(knob->GetNodeName(), "depth");
  EXPECT_EQ(knob->GetComponentNumber(), 4);
  EXPECT_EQ(knob->GetInstanceId(), 10);
  EXPECT_EQ(knob->GetChannel(), ChannelType::CAN);

  delete module;
  delete node_desc;
}
}  // namespace models
}  // namespace analog3
