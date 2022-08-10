#include "rnd/xoshiro.hpp"

#include "gtest.hpp"

TEST(Xoshiro, Rotl1) { ASSERT_EQ(rnd::rotl<1>(42), static_cast<rnd::t>(84)); }

TEST(Xoshiro, StartSameEveryTimeNext) {
  ASSERT_EQ(rnd::next(), static_cast<rnd::t>(5987356902031041503));
}

TEST(Xoshiro, StartSameEveryTimeBit) { ASSERT_EQ(rnd::bit(), true); }

TEST(Xoshiro, BitRefill) {
  uint8_t i = 0;
  do { rnd::bit(); } while (++i);
}
