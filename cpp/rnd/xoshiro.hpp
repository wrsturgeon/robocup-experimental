#pragma once

// Some edits to Blackman & Vigna's xoshiro PRNGs.
// See the below 64b & 32b sections for links.

#include "util/ints.hpp"
#include "util/ternary.hpp"

#include <array>
#include <cstdint>

namespace rnd {

// 64b: https://prng.di.unimi.it/xoshiro256plusplus.c
// 32b: https://prng.di.unimi.it/xoshiro128plusplus.c
using t = cint<kSystemBits, unsigned>;  // rnd::t
inline constexpr t s1 = if32(0x7b1dcdaf, 0xe220a8397b1dcdaf);
inline constexpr t s2 = if32(0xa1b965f4, 0x6e789e6aa1b965f4);
inline constexpr t s3 = if32(0x8009454f, 0x06c45d188009454f);
inline constexpr t s4 = if32(0x724c81ec, 0xf88bb8a8724c81ec);
inline constexpr u8 r1 = if32(7, 23);
inline constexpr u8 l1 = if32(9, 17);
inline constexpr u8 r2 = if32(11, 45);

template <u8 k> pure static auto
rotl(const t x) -> t {
  return (x << k) | (x >> (kSystemBits - k));
}

[[nodiscard]] static inline auto
next() -> t {
  // NOLINTBEGIN(readability-magic-numbers)

  static auto s = std::array<t, 4>{s1, s2, s3, s4};

  auto const result = t{rotl<r1>(s[0] + s[3]) + s[0]};

  auto const tmp = t{s[1] << l1};

  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];

  s[2] ^= tmp;

  s[3] = rotl<r2>(s[3]);

  return result;

  // NOLINTEND(readability-magic-numbers)
}

// [[nodiscard]] static auto bit() -> bool {
//   static t state;
//   static u8 uses;
//   if (!uses) {
//     uses = kSystemBits - 1;
//     state = next();
//   } else {
//     --uses;
//   }
//   auto const rtn = static_cast<bool>(state & 1);
//   state >>= 1;
//   return rtn;
// }

}  // namespace rnd
