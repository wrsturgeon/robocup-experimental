#pragma once

#include "util/options.hpp"

template <auto val_if_32, auto val_if_64>
static constexpr inline __attribute__((always_inline)) auto
if32() -> auto{
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  if constexpr (kSystemBits == 32) { return val_if_32; }
  return val_if_64;
}
