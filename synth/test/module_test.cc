#include "gtest/gtest.h"
#include "synth/module.h"

namespace analog3 {

TEST(SynthNodeTest, Basic) {
  SynthNode* node = new Module();
  EXPECT_EQ(node->GetNodeType(), NodeType::kNodeTypeModule);
}

}  // namespace analog3
