#include "wasserstein/pyramid.hpp"
#include "gtest.hpp"

TEST(Pyramid, pyrsize00) { ASSERT_EQ(wasserstein::pyrsize(0, 0), static_cast<size_t>(0)); }
