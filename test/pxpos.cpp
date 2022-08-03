#include "vision/pxpos.hpp"
#include "gtest.hpp"

TEST(PxPos, PxPos000) { ASSERT_EQ((vision::pxpos_t{0, 0}.r2()), static_cast<uint32_t>(0)); }
TEST(PxPos, PxPos345) { ASSERT_EQ((vision::pxpos_t{3, 4}.r2()), static_cast<uint32_t>(5)); }
