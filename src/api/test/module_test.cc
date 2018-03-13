#include "gtest/gtest.h"
#include "api/module.h"

namespace analog3 {

TEST(SynthNodeTest, Basic) {
  models::SynthNode* node = new models::Module();
  EXPECT_EQ(node->GetNodeType(), models::NodeType::kNodeTypeModule);
}

}  // namespace analog3
