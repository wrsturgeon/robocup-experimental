#include "vision/image-api.hpp"
#include "gtest.hpp"

TEST(ImageAPI, ConstructorWithoutIssue) { ASSERT_NO_THROW((vision::NaoImage<42, 13>{})); }
