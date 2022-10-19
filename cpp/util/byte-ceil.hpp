#pragma once

#include "util/ints.hpp"

#include <bit>

pure static auto
byte_ceil(u8 n) -> u8 {
  if (n <= 8) { return 8; }  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return std::bit_ceil(n);
}
