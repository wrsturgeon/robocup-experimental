#pragma once

// Some edits to Blackman & Vigna's xoshiro PRNGs.
// See the below 64b & 32b sections for links.

#include "util/ternary.hpp"

#include <tuple>

namespace rnd {

// 64b: https://prng.di.unimi.it/xoshiro256plusplus.c
// 32b: https://prng.di.unimi.it/xoshiro128plusplus.c
using t = ufull_t;  // rnd::t

template <u8 k>
pure static auto
rotl(const t x) -> t {
  return (x << k) | (x >> (kSystemBits - k));
}

// NOLINTBEGIN(readability-magic-numbers)
impure static auto
next() -> t {
  static auto s = std::tuple<t, t, t, t>{
        if32(0x7b1dcdaf, 0xe220a8397b1dcdaf),  //
        if32(0xa1b965f4, 0x6e789e6aa1b965f4),
        if32(0x8009454f, 0x06c45d188009454f),
        if32(0x724c81ec, 0xf88bb8a8724c81ec)};

  auto const result = t{rotl<if32(7, 23)>(std::get<0>(s) + std::get<3>(s)) + std::get<0>(s)};

  auto const tmp = t{std::get<1>(s) << if32(9, 17)};

  std::get<2>(s) ^= std::get<0>(s);
  std::get<3>(s) ^= std::get<1>(s);
  std::get<1>(s) ^= std::get<2>(s);
  std::get<0>(s) ^= std::get<3>(s);

  std::get<2>(s) ^= tmp;

  std::get<3>(s) = rotl<if32(11, 45)>(std::get<3>(s));

  return result;
}
// NOLINTEND(readability-magic-numbers)

// impure static auto bit() -> bool {
//   static t state;
//   static u8 uses;
//   if (uses == 0) {
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
