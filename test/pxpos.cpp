#include "vision/pxpos.hpp"

#include "gtest.hpp"

TEST(PxPos, PxPos000) {
  ASSERT_EQ((vision::px2d{0, 0}.r2()), static_cast<vision::pxrsq_t>(0));
}

TEST(PxPos, PxPos345) {
  ASSERT_EQ((vision::px2d{3, 4}.r2()), static_cast<vision::pxrsq_t>(25));
}

TEST(PxPos, String) { ASSERT_EQ(static_cast<std::string>(vision::px2d{3, 4}), "(3x, 4y)"); }

TEST(PxPos, Stream) {
  std::ostream os{nullptr};
  ASSERT_NO_THROW(os << (vision::px2d{3, 4}));
}
