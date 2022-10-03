#include "ml/optimizer.hpp"

#include "gtest.hpp"

#include <cstdint>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

using T = fp::t<32, 30, signed>;

TEST(Optimizer, ZeroStep) { ASSERT_FALSE((ml::Adam<T>{}.step(T::zero()))); }

TEST(Optimizer, ZeroStepWD) { ASSERT_FALSE((ml::Adam<T>{}.step(T::zero(), T::zero()))); }

TEST(Optimizer, EventuallyDecayToZero) {
  auto adam = ml::Adam<T, false, 10, 1, 2>{};
  auto zero = T::zero();
  auto w = T::zero();
  std::uint8_t i = 0;
#pragma unroll  // NOLINTNEXTLINE(altera-unroll-loops)
  do {
    w -= adam.step(zero);
  } while (++i);
  ASSERT_FALSE(adam.step(zero));
}

TEST(Optimizer, RepublicanDecayToZero) {
  auto adam = ml::Adam<T, true, 10, 1, 2>{};
  auto zero = T::zero();
  auto w = T::zero();
  std::uint8_t i = 0;
#pragma unroll  // NOLINTNEXTLINE(altera-unroll-loops)
  do {
    w -= adam.step(zero);
  } while (++i);
  ASSERT_FALSE(adam.step(zero));
}

TEST(Optimizer, WithWeightDecay) {
  auto adam = ml::Adam<T, false, 10, 1, 2, 3>{};
  auto zero = T::zero();
  auto w = T::unit();
  std::uint8_t i = 0;
#pragma unroll  // NOLINTNEXTLINE(altera-unroll-loops)
  do {
    w -= adam.step(zero, w);
    std::cout << w.expose() << std::endl;
  } while (++i);
  ASSERT_FALSE(w);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
