#include "gtest/gtest.h"
#include "wasserstein/pyramid.hpp"

TEST(pyramid, pyrsize_zero) { ASSERT_EQ(static_cast<size_t>(0), wasserstein::pyrsize(0, 0)); }

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
