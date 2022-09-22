#include "vision/infer-3d.hpp"

#include "gtest.hpp"

TEST(Infer3D, ProjectionLossNoThrow) { ASSERT_NO_THROW(vision::projection_loss()); }
