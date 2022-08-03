#include "vision/image-api.hpp"
#include "gtest.hpp"

TEST(ImageAPI, NaoImageW) { ASSERT_EQ((vision::NaoImage<42, 13>{}.width()), 42); }
TEST(ImageAPI, NaoImageH) { ASSERT_EQ((vision::NaoImage<9, 42>{}.height()), 42); }
