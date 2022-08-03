#include "vision/distortion.hpp"
#include "gtest.hpp"

TEST(Distortion, LensNoArgs) { ASSERT_NO_THROW(vision::Lens{}); }
