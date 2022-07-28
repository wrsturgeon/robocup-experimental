#include "gtest.hpp"
#include "wasserstein/pyramid.hpp"

TEST(pyramid, pyrsize_00) { ASSERT_EQ(static_cast<size_t>(0), wasserstein::pyrsize(0, 0)); }
TEST(pyramid, pyrsize_10) { ASSERT_EQ(static_cast<size_t>(0), wasserstein::pyrsize(1, 0)); }
TEST(pyramid, pyrsize_01) { ASSERT_EQ(static_cast<size_t>(0), wasserstein::pyrsize(0, 1)); }
TEST(pyramid, pyrsize_11) { ASSERT_EQ(static_cast<size_t>(1), wasserstein::pyrsize(1, 1)); }
TEST(pyramid, pyrsize_huge) { ASSERT_EQ(static_cast<size_t>(87381), wasserstein::pyrsize(256, 256)); }
TEST(pyramid, pyrsize_thin) { ASSERT_EQ(static_cast<size_t>(256), wasserstein::pyrsize(256, 1)); }
TEST(pyramid, pyrsize_match) { ASSERT_EQ(sizeof(wasserstein::Pyramid<1024, 1024>), wasserstein::pyrsize(1024, 1024)); }
TEST(pyramid, init_pyramid_equal) { uint8_t i[8][8]; ASSERT_NO_THROW((wasserstein::Pyramid<8, 8>{i})); }
TEST(pyramid, init_pyramid_halfh) { uint8_t i[4][8]; ASSERT_NO_THROW((wasserstein::Pyramid<8, 4>{i})); }
TEST(pyramid, init_pyramid_halfw) { uint8_t i[8][4]; ASSERT_NO_THROW((wasserstein::Pyramid<4, 8>{i})); }
