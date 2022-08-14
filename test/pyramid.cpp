#include "wasserstein/pyramid.hpp"

#include "gtest.hpp"

#include <cstdint>

TEST(Pyramid, OneByOne) {
  auto a = wasserstein::Array<1, 1>::Constant(42);
  auto p = wasserstein::Pyramid<1, 1>{a};
  ASSERT_EQ(p(0, 0), static_cast<std::uint8_t>(42));
}

TEST(Pyramid, TwoByOne) {
  auto a = wasserstein::Array<2, 1>::Constant(42);
  auto p = wasserstein::Pyramid<2, 1>{a};
  ASSERT_EQ(p(0, 0), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(1, 0), static_cast<std::uint8_t>(42));
}

TEST(Pyramid, OneByTwo) {
  auto a = wasserstein::Array<1, 2>::Constant(42);
  auto p = wasserstein::Pyramid<1, 2>{a};
  ASSERT_EQ(p(0, 0), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(0, 1), static_cast<std::uint8_t>(42));
}

TEST(Pyramid, TwoByTwo) {
  auto a = wasserstein::Array<2, 2>::Constant(42);
  auto p = wasserstein::Pyramid<2, 2>{a};
  ASSERT_EQ(p(0, 0), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(0, 1), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(1, 0), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p(1, 1), static_cast<std::uint8_t>(42));
  ASSERT_EQ(p.dn()(0, 0), static_cast<std::uint8_t>(42));
}
