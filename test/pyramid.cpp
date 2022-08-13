#include "wasserstein/pyramid.hpp"

#include "gtest.hpp"

#include <cstdint>

TEST(Pyramid, Singleton) {
  auto a = Eigen::Array<uint8_t, 1, 1>::Constant(42);
  auto p = wasserstein::Pyramid<1, 1>{a};
  ASSERT_EQ(p(0, 0), static_cast<std::uint8_t>(42));
}

TEST(Pyramid, TwoByTwo) {
  auto a = Eigen::Array<uint8_t, 2, 2>::Constant(42);
  auto p = wasserstein::Pyramid<2, 2>{a};
  ASSERT_EQ(p.dn(0, 0), static_cast<std::uint8_t>(42));
}
