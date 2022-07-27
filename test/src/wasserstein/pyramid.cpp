#include "gtest/gtest.h"
#include "wasserstein/pyramid.hpp"

TEST(pyramid, pyrsize_00) { ASSERT_EQ(static_cast<size_t>(0), wasserstein::pyrsize(0, 0)); }
TEST(pyramid, pyrsize_10) { ASSERT_EQ(static_cast<size_t>(0), wasserstein::pyrsize(1, 0)); }
