#include "vision/distortion.hpp"

#include "gtest.hpp"

TEST(Distortion, LensZeros) { ASSERT_FALSE(vision::Lens{}.redistort(px2d::zero())); }
