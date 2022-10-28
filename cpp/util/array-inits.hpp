#pragma once

#include "util/ints.hpp"
#include "util/uninitialized.hpp"

#include <array>

template <typename T, ufull_t N> pure static auto
zeros() -> std::array<T, N> {
  std::array<T, N> a;  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
  a.fill(0);
  return a;
}

template <typename T, ufull_t N> pure static auto
ones() -> std::array<T, N> {
  std::array<T, N> a;  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
  a.fill(1);
  return a;
}
