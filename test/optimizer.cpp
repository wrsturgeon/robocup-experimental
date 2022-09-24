#include "ml/optimizer.hpp"

#include "gtest.hpp"

#include <cstdint>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

TEST(Optimizer, ZeroStep) {
  ASSERT_FALSE((ml::Adam<10, 27, 3, 10>{}.step(fp<32, 32, unsigned>{})));
}
TEST(Optimizer, EventuallyDecayToZero) {
  ml::Adam<10, 27, 1, 2> adam;
  auto zero = fp<32, 32, unsigned>{};
  for (std::uint8_t i = 0; ++i;) { adam.step(zero); }
  ASSERT_FALSE(adam.step(zero));
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
