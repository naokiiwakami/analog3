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

TEST(SynthNodeTest, DecodeFromApiMessageModuleSingle) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::MODULE);
  node_desc->set_node_name("test_module");
  node_desc->set_component_number(2);
  node_desc->set_channel(api::SynthNode::CAN);
  node_desc->mutable_module()->set_model_id(800);
  node_desc->mutable_module()->set_model_name("monomonopolypoly");

  // decode
  SynthNode* node = SynthNode::Decode(*node_desc);

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

TEST(SynthNodeTest, DecodeFromApiMessageKnobSingle) {
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

  // decode
  SynthNode* node = SynthNode::Decode(*node_desc);

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

TEST(SynthNodeTest, DecodeFromApiMessageKnobSingleDefault) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::KNOB);
  node_desc->set_node_name("test_knob_default");

  // decode
  SynthNode* node = SynthNode::Decode(*node_desc);

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

TEST(SynthNodeTest, DecodeFromApiMessageSwitchSingle) {
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

  // decode
  SynthNode* node = SynthNode::Decode(*node_desc);

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

TEST(SynthNodeTest, DecodeFromApiMessageSwitchSingleDefault) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::SWITCH);
  node_desc->set_node_name("test_switch_default");

  // decode
  SynthNode* node = SynthNode::Decode(*node_desc);

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

TEST(SynthNodeTest, DecodeFromApiMessageSwitchType) {
  // make the source message
  api::SynthNode* node_desc = new api::SynthNode();
  node_desc->set_node_type(api::SynthNode::SWITCH);
  node_desc->set_node_name("test_switch_types");

  node_desc->mutable_switch_()->set_switch_type(api::Switch::TOGGLE);
  SynthNode* node = SynthNode::Decode(*node_desc);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::SWITCH);
  Switch* switch_ = dynamic_cast<Switch*>(node);
  ASSERT_NE(switch_, nullptr);
  EXPECT_EQ(switch_->GetSwitchType(), SwitchType::TOGGLE);
  delete switch_;

  node_desc->mutable_switch_()->set_switch_type(api::Switch::MOMENTARY);
  node = SynthNode::Decode(*node_desc);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::SWITCH);
  switch_ = dynamic_cast<Switch*>(node);
  ASSERT_NE(switch_, nullptr);
  EXPECT_EQ(switch_->GetSwitchType(), SwitchType::MOMENTARY);
  delete switch_;

  node_desc->mutable_switch_()->set_switch_type(api::Switch::SELECTOR);
  node = SynthNode::Decode(*node_desc);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->GetNodeType(), NodeType::SWITCH);
  switch_ = dynamic_cast<Switch*>(node);
  ASSERT_NE(switch_, nullptr);
  EXPECT_EQ(switch_->GetSwitchType(), SwitchType::SELECTOR);
  delete switch_;

  delete node_desc;
}

TEST(SynthNodeTest, DecodeFromApiMessageModuleNested) {
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

  // decode
  SynthNode* node = SynthNode::Decode(*node_desc);

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

TEST(SynthNodeTest, EncodeModule) {
  Module module;
  module.SetNodeName("instance1");
  module.SetComponentNumber(30);
  module.SetChannel(ChannelType::CAN);
  module.SetInstanceId(40);
  module.SetInitialValue(65535);
  //
  module.SetModelName("test_module");
  module.SetModelId(123);

  api::SynthNode* node_desc = new api::SynthNode();
  module.Encode(node_desc);

  EXPECT_EQ(node_desc->node_type(), api::SynthNode::MODULE);
  EXPECT_EQ(node_desc->node_name(), "instance1");
  EXPECT_EQ(node_desc->component_number(), 30);
  EXPECT_EQ(node_desc->channel(), api::SynthNode::CAN);
  EXPECT_EQ(node_desc->instance_id(), 40);
  EXPECT_EQ(node_desc->initial_value(), 65535);
  EXPECT_TRUE(node_desc->has_module());
  EXPECT_FALSE(node_desc->has_knob());
  EXPECT_FALSE(node_desc->has_switch_());
  EXPECT_EQ(node_desc->module().model_name(), "test_module");
  EXPECT_EQ(node_desc->module().model_id(), 123);

  SynthNode* node = SynthNode::Decode(*node_desc);
  ASSERT_NE(node, nullptr);
  Module* decoded = dynamic_cast<Module*>(node);
  ASSERT_NE(decoded, nullptr);
  ASSERT_EQ(decoded->GetNodeName(), module.GetNodeName());
  ASSERT_EQ(decoded->GetComponentNumber(), module.GetComponentNumber());
  ASSERT_EQ(decoded->GetChannel(), module.GetChannel());
  ASSERT_EQ(decoded->GetInstanceId(), module.GetInstanceId());
  ASSERT_EQ(decoded->GetInitialValue(), module.GetInitialValue());
  ASSERT_EQ(decoded->GetModelName(), module.GetModelName());
  ASSERT_EQ(decoded->GetModelId(), module.GetModelId());

  delete decoded;
  delete node_desc;
}

TEST(SynthNodeTest, EncodeKnob) {
  Knob knob;
  knob.SetNodeName("attack");
  knob.SetComponentNumber(255);
  knob.SetChannel(ChannelType::NONE);
  knob.SetInstanceId(65535);
  knob.SetMinValue(333);
  knob.SetOffset(32768);
  knob.SetScale(3);

  api::SynthNode* node_desc = new api::SynthNode();
  knob.Encode(node_desc);

  ASSERT_EQ(node_desc->node_type(), api::SynthNode::KNOB);
  ASSERT_EQ(node_desc->node_name(), "attack");
  ASSERT_EQ(node_desc->component_number(), 255);
  ASSERT_EQ(node_desc->channel(), api::SynthNode::NONE);
  ASSERT_EQ(node_desc->instance_id(), 65535);
  EXPECT_FALSE(node_desc->has_module());
  EXPECT_TRUE(node_desc->has_knob());
  EXPECT_FALSE(node_desc->has_switch_());
  EXPECT_EQ(node_desc->knob().min_value(), 333);
  EXPECT_EQ(node_desc->knob().max_value(), 65535);
  EXPECT_EQ(node_desc->knob().offset(), 32768);
  EXPECT_EQ(node_desc->knob().scale(), 3);

  SynthNode* node = SynthNode::Decode(*node_desc);
  ASSERT_NE(node, nullptr);
  Knob* decoded = dynamic_cast<Knob*>(node);
  ASSERT_NE(decoded, nullptr);

  delete decoded;
  delete node_desc;
}

TEST(SynthNodeTest, EncodeSwitch) {
  Switch switch_;
  switch_.SetSwitchType(SwitchType::TOGGLE);

  api::SynthNode* node_desc = new api::SynthNode();
  switch_.Encode(node_desc);

  ASSERT_EQ(node_desc->node_type(), api::SynthNode::SWITCH);
  EXPECT_FALSE(node_desc->has_module());
  EXPECT_FALSE(node_desc->has_knob());
  EXPECT_TRUE(node_desc->has_switch_());
  EXPECT_EQ(node_desc->switch_().switch_type(), api::Switch::TOGGLE);
  EXPECT_EQ(node_desc->switch_().options_size(), 2);
  EXPECT_EQ(node_desc->switch_().options(0), "off");
  EXPECT_EQ(node_desc->switch_().options(1), "on");

  SynthNode* node = SynthNode::Decode(*node_desc);
  ASSERT_NE(node, nullptr);
  Switch* decoded = dynamic_cast<Switch*>(node);
  ASSERT_NE(decoded, nullptr);

  EXPECT_EQ(decoded->GetSwitchType(), SwitchType::TOGGLE);
  EXPECT_EQ(decoded->GetOptions().size(), 2);
  EXPECT_EQ(decoded->GetOptions()[0], "off");
  EXPECT_EQ(decoded->GetOptions()[1], "on");

  delete decoded;

  node_desc->Clear();
  switch_.SetSwitchType(SwitchType::ROTARY);
  switch_.ClearOptions();
  switch_.AddOption("Triangular");
  switch_.AddOption("Sawtooth");
  switch_.AddOption("Pulse");
  switch_.Encode(node_desc);
  EXPECT_EQ(node_desc->switch_().switch_type(), api::Switch::ROTARY);
  EXPECT_EQ(node_desc->switch_().options_size(), 3);
  decoded = dynamic_cast<Switch*>(SynthNode::Decode(*node_desc));
  ASSERT_NE(decoded, nullptr);
  EXPECT_EQ(decoded->GetSwitchType(), SwitchType::ROTARY);
  EXPECT_EQ(decoded->GetOptions().size(), 3);
  EXPECT_EQ(decoded->GetOptions()[0], "Triangular");
  EXPECT_EQ(decoded->GetOptions()[1], "Sawtooth");
  EXPECT_EQ(decoded->GetOptions()[2], "Pulse");
  delete decoded;

  node_desc->Clear();
  switch_.SetSwitchType(SwitchType::SELECTOR);
  switch_.ClearOptions();
  switch_.AddOption("1");
  switch_.AddOption("2");
  switch_.AddOption("3");
  switch_.AddOption("4");
  switch_.AddOption("5");
  switch_.Encode(node_desc);
  EXPECT_EQ(node_desc->switch_().switch_type(), api::Switch::SELECTOR);
  EXPECT_EQ(node_desc->switch_().options_size(), 5);
  decoded = dynamic_cast<Switch*>(SynthNode::Decode(*node_desc));
  ASSERT_NE(decoded, nullptr);
  EXPECT_EQ(decoded->GetSwitchType(), SwitchType::SELECTOR);
  EXPECT_EQ(decoded->GetOptions().size(), 5);
  EXPECT_EQ(decoded->GetOptions()[0], "1");
  EXPECT_EQ(decoded->GetOptions()[1], "2");
  EXPECT_EQ(decoded->GetOptions()[2], "3");
  EXPECT_EQ(decoded->GetOptions()[3], "4");
  EXPECT_EQ(decoded->GetOptions()[4], "5");
  delete decoded;

  node_desc->Clear();
  switch_.SetSwitchType(SwitchType::MOMENTARY);
  switch_.ClearOptions();
  switch_.AddOption("off");
  switch_.AddOption("on");
  switch_.Encode(node_desc);
  EXPECT_EQ(node_desc->switch_().switch_type(), api::Switch::MOMENTARY);
  EXPECT_EQ(node_desc->switch_().options_size(), 2);
  decoded = dynamic_cast<Switch*>(SynthNode::Decode(*node_desc));
  ASSERT_NE(decoded, nullptr);
  EXPECT_EQ(decoded->GetSwitchType(), SwitchType::MOMENTARY);
  EXPECT_EQ(decoded->GetOptions().size(), 2);
  EXPECT_EQ(decoded->GetOptions()[0], "off");
  EXPECT_EQ(decoded->GetOptions()[1], "on");
  delete decoded;

  delete node_desc;
}

TEST(SynthNodeTest, EncodeNestedModule) {
  Module module;
  module.SetNodeName("instance1");
  module.SetModelName("envelope generator");
  module.SetChannel(ChannelType::CAN);
  module.SetInstanceId(40);

  Knob* attack = new Knob();
  attack->SetNodeName("attack time");
  attack->SetComponentNumber(1);
  attack->SetMinValue(0);
  attack->SetMaxValue(1023);
  attack->SetInitialValue(0);
  module.AddChild(attack);
  EXPECT_EQ(attack->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(attack->GetInstanceId(), 40);

  Knob* sustain = new Knob();
  sustain->SetNodeName("sustain level");
  sustain->SetComponentNumber(2);
  sustain->SetMinValue(0);
  sustain->SetMaxValue(1023);
  sustain->SetInitialValue(700);
  module.AddChild(sustain);
  EXPECT_EQ(sustain->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(sustain->GetInstanceId(), 40);

  Switch* curve = new Switch();
  curve->SetNodeName("curve");
  curve->SetComponentNumber(3);
  curve->ClearOptions();
  curve->AddOption("linear");
  curve->AddOption("exponential");
  module.AddChild(curve);
  EXPECT_EQ(curve->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(curve->GetInstanceId(), 40);

  api::SynthNode* node_desc = new api::SynthNode();
  module.Encode(node_desc);

  EXPECT_EQ(node_desc->node_type(), api::SynthNode::MODULE);
  EXPECT_EQ(node_desc->instance_id(), 40);
  EXPECT_EQ(node_desc->component_number(), 0);
  EXPECT_EQ(node_desc->child_nodes_size(), 3);
  EXPECT_EQ(node_desc->child_nodes(0).node_type(), api::SynthNode::KNOB);
  EXPECT_EQ(node_desc->child_nodes(0).node_name(), "attack time");
  EXPECT_EQ(node_desc->child_nodes(0).initial_value(), 0);
  ASSERT_EQ(node_desc->child_nodes(0).has_knob(), true);
  EXPECT_EQ(node_desc->child_nodes(1).node_type(), api::SynthNode::KNOB);
  EXPECT_EQ(node_desc->child_nodes(1).node_name(), "sustain level");
  EXPECT_EQ(node_desc->child_nodes(1).initial_value(), 700);
  ASSERT_EQ(node_desc->child_nodes(1).has_knob(), true);
  EXPECT_EQ(node_desc->child_nodes(2).node_type(), api::SynthNode::SWITCH);
  EXPECT_EQ(node_desc->child_nodes(2).node_name(), "curve");

  Module* decoded_module = dynamic_cast<Module*>(SynthNode::Decode(*node_desc));
  ASSERT_NE(decoded_module, nullptr);
  EXPECT_EQ(decoded_module->GetNodeName(), module.GetNodeName());
  EXPECT_EQ(decoded_module->GetModelName(), module.GetModelName());
  EXPECT_EQ(decoded_module->GetChannel(), module.GetChannel());
  EXPECT_EQ(decoded_module->GetInstanceId(), module.GetInstanceId());
  ASSERT_EQ(decoded_module->GetChildren().size(), 3);
  Knob* decoded_attack = dynamic_cast<Knob*>(decoded_module->GetChildren()[0]);
  ASSERT_NE(decoded_attack, nullptr);
  EXPECT_EQ(decoded_attack->GetNodeName(), "attack time");
  EXPECT_EQ(decoded_attack->GetComponentNumber(), 1);
  EXPECT_EQ(decoded_attack->GetMinValue(), 0);
  EXPECT_EQ(decoded_attack->GetMaxValue(), 1023);
  EXPECT_EQ(decoded_attack->GetInitialValue(), 0);
  EXPECT_EQ(decoded_attack->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(decoded_attack->GetInstanceId(), 40);
  Knob* decoded_sustain = dynamic_cast<Knob*>(decoded_module->GetChildren()[1]);
  ASSERT_NE(decoded_sustain, nullptr);
  EXPECT_EQ(decoded_sustain->GetNodeName(), "sustain level");
  EXPECT_EQ(decoded_sustain->GetComponentNumber(), 2);
  EXPECT_EQ(decoded_sustain->GetMinValue(), 0);
  EXPECT_EQ(decoded_sustain->GetMaxValue(), 1023);
  EXPECT_EQ(decoded_sustain->GetInitialValue(), 700);
  EXPECT_EQ(decoded_sustain->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(decoded_sustain->GetInstanceId(), 40);
  Switch* decoded_curve = dynamic_cast<Switch*>(decoded_module->GetChildren()[2]);
  ASSERT_NE(decoded_curve, nullptr);
  EXPECT_EQ(decoded_curve->GetNodeName(), "curve");
  EXPECT_EQ(decoded_curve->GetComponentNumber(), 3);
  ASSERT_EQ(decoded_curve->GetOptions().size(), 2);
  EXPECT_EQ(decoded_curve->GetOptions()[0], "linear");
  EXPECT_EQ(decoded_curve->GetOptions()[1], "exponential");
  EXPECT_EQ(decoded_curve->GetChannel(), ChannelType::CAN);
  EXPECT_EQ(decoded_curve->GetInstanceId(), 40);

  delete decoded_module;
  delete node_desc;
}

}  // namespace models
}  // namespace analog3
