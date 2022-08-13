#pragma once

// Some edits to Blackman & Vigna's xoshiro PRNGs.
// See the below 64b & 32b sections for links.

#include "util/custom-ints.hpp"
#include "util/ternary-bits.hpp"

#include <array>
#include <cstdint>

namespace rnd {

// 64b: https://prng.di.unimi.it/xoshiro256plusplus.c
// 32b: https://prng.di.unimi.it/xoshiro128plusplus.c
using t = custom_int<kSystemBits>::unsigned_t;  // rnd::t
static constexpr t s1 = if32<0x7b1dcdaf, 0xe220a8397b1dcdaf>();
static constexpr t s2 = if32<0xa1b965f4, 0x6e789e6aa1b965f4>();
static constexpr t s3 = if32<0x8009454f, 0x06c45d188009454f>();
static constexpr t s4 = if32<0x724c81ec, 0xf88bb8a8724c81ec>();
static constexpr std::uint8_t r1 = if32<7, 23>();
static constexpr std::uint8_t l1 = if32<9, 17>();
static constexpr std::uint8_t r2 = if32<11, 45>();

template <std::uint8_t k>
static inline constexpr auto
rotl(const t x) -> t {
  return (x << k) | (x >> (kSystemBits - k));
}

auto
next() -> t {
  // NOLINTBEGIN(readability-magic-numbers)

  static auto s = std::array<t, 4>{s1, s2, s3, s4};

  t const result = rotl<r1>(s[0] + s[3]) + s[0];

  t const tmp = s[1] << l1;

  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];

  s[2] ^= tmp;

  s[3] = rotl<r2>(s[3]);

  return result;

  // NOLINTEND(readability-magic-numbers)
}

auto
bit() -> bool {
  static t state;
  static std::uint8_t uses;
  if (!uses) {
    uses = kSystemBits - 1;
    state = next();
  } else {
    --uses;
  }
  bool const rtn = static_cast<bool>(state);  // === (state & 1)
  state >>= 1;
  return rtn;
}

}  // namespace rnd
