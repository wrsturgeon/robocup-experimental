#include "vision/pxpos.hpp"

#include "gtest.hpp"

TEST(PxPos, PxPos000) {
  ASSERT_EQ((vision::pxpos_t{0, 0}.r2()), static_cast<vision::pxr2_t>(0));
}

TEST(PxPos, PxPos345) {
  ASSERT_EQ((vision::pxpos_t{3, 4}.r2()), static_cast<vision::pxr2_t>(25));
}

TEST(PxPos, String) { ASSERT_EQ(static_cast<std::string>(vision::pxpos_t{3, 4}), "(3x, 4y)"); }

TEST(PxPos, Stream) {
  std::ostream os{nullptr};
  ASSERT_NO_THROW(os << (vision::pxpos_t{3, 4}));
}
