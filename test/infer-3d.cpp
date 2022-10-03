#include "vision/infer-3d.hpp"

#include "gtest.hpp"

TEST(Infer3D, ProjectionLossNoThrow) { ASSERT_FALSE(vision::projection_loss()); }
