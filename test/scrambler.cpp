#include "ml/scrambler.hpp"

#include "gtest.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-goto,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

TEST(Scrambler, ByteAddress) { ASSERT_NO_THROW((ml::Scrambler<uint8_t, 8>{})); }

TEST(Scrambler, DoYourBasicThing) {
  auto s = ml::Scrambler<std::uint8_t, 8>{};
  std::uint8_t i = 0;
#pragma unroll  // NOLINTNEXTLINE(altera-unroll-loops)
  do {
    ASSERT_NO_THROW(s.prefill(static_cast<volatile std::uint8_t>(42)));
  } while (++i);
#pragma unroll  // NOLINTNEXTLINE(altera-unroll-loops)
  do {
    ASSERT_EQ(s.store_and_recall(42), 42);
  } while (++i);
}

// NOLINTEND(cppcoreguidelines-avoid-goto,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
