#include "gtest/gtest.h"
#include "wasserstein/pyramid.hpp"

TEST(pyramid, pyrsize_zero) { ASSERT_EQ(static_cast<size_t>(0), wasserstein::pyrsize(0, 0)); }
