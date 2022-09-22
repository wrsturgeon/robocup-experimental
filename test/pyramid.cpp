#include "vision/pyramid.hpp"

#include "gtest.hpp"

#include <cstdint>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

TEST(Pyramid, OneByOne) {
  auto a = Array<1, 1>::Constant(42);
  auto p = vision::Pyramid<1, 1>{a};
  ASSERT_EQ(p(0, 0), static_cast<std::uint8_t>(42));
}

TEST(Pyramid, TwoByOne) {
  auto a = Array<2, 1>::Constant(42);
  auto p = vision::Pyramid<2, 1>{a};
  ASSERT_EQ(p(0, 0), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(1, 0), static_cast<std::uint8_t>(42));
}

TEST(Pyramid, OneByTwo) {
  auto a = Array<1, 2>::Constant(42);
  auto p = vision::Pyramid<1, 2>{a};
  ASSERT_EQ(p(0, 0), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(0, 1), static_cast<std::uint8_t>(42));
}

TEST(Pyramid, TwoByTwo) {
  auto a = Array<2, 2>::Constant(42);
  auto p = vision::Pyramid<2, 2>{a};
  ASSERT_EQ(p(0, 0), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(0, 1), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(1, 0), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(1, 1), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p.dn(0, 0), static_cast<std::uint8_t>(42));
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
