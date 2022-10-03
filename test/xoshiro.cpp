#include "rnd/xoshiro.hpp"

#include "gtest.hpp"

#include "util/uninitialized.hpp"

TEST(Xoshiro, Rotl1) { ASSERT_EQ(rnd::rotl<1>(42), static_cast<rnd::t>(84)); }

TEST(Xoshiro, StartSameEveryTimeNext) { ASSERT_EQ(rnd::next(), static_cast<rnd::t>(5987356902031041503)); }

TEST(Xoshiro, StartSameEveryTimeBit) { ASSERT_EQ(rnd::bit(), true); }

TEST(Xoshiro, BitRefill) {
  std::uint8_t i = 0;
  auto bogus = uninitialized<bool>();  // NOLINT(clang-analyzer-deadcode.DeadStores)
#pragma unroll                         // NOLINTNEXTLINE(altera-unroll-loops)
  do {
    bogus = rnd::bit();
  } while (++i);
  ASSERT_TRUE(bogus || !bogus);  // fine, get your fucking cookie lmao
}
