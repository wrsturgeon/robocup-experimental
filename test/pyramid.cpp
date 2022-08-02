#include "wasserstein/pyramid.hpp"
#include "gtest.hpp"

TEST(Pyramid, pyrsize00) { ASSERT_EQ(wasserstein::pyrsize(0, 0), static_cast<size_t>(0)); }
TEST(Pyramid, pyrsize01) { ASSERT_EQ(wasserstein::pyrsize(0, 1), static_cast<size_t>(0)); }
TEST(Pyramid, pyrsize10) { ASSERT_EQ(wasserstein::pyrsize(1, 0), static_cast<size_t>(0)); }
TEST(Pyramid, pyrsize11) { ASSERT_EQ(wasserstein::pyrsize(1, 1), static_cast<size_t>(1)); }
TEST(Pyramid, pyrsize22) { ASSERT_EQ(wasserstein::pyrsize(2, 2), static_cast<size_t>(5)); }
TEST(Pyramid, pyrsize_huge) { ASSERT_EQ(wasserstein::pyrsize(256, 256), static_cast<size_t>((256*256)+(128*128)+(64*64)+(32*32)+(16*16)+(8*8)+(4*4)+(2*2)+(1*1))); }
TEST(Pyramid, pyrsize_plus1) { ASSERT_EQ(wasserstein::pyrsize(257, 256), wasserstein::pyrsize(256, 256) + 256); }
