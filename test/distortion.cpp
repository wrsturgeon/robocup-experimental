#include "vision/distortion.hpp"

#include "gtest.hpp"

TEST(Distortion, LensZeros) { ASSERT_NO_THROW((vision::Lens{})); }
