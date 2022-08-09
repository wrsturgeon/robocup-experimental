#include "rnd/xoshiro.hpp"

namespace rnd {

#if BITS == 64  // https://prng.di.unimi.it/xoshiro256plusplus.c
static constexpr t s1 = 0xe220a8397b1dcdaf;
static constexpr t s2 = 0x6e789e6aa1b965f4;
static constexpr t s3 = 0x06c45d188009454f;
static constexpr t s4 = 0xf88bb8a8724c81ec;
static constexpr std::uint8_t r1 = 23;
static constexpr std::uint8_t l1 = 17;
static constexpr std::uint8_t r2 = 45;
#elif BITS == 32  // https://prng.di.unimi.it/xoshiro128plusplus.c
static constexpr t s1 = 0x7b1dcdaf;
static constexpr t s2 = 0xa1b965f4;
static constexpr t s3 = 0x8009454f;
static constexpr t s4 = 0x724c81ec;
static constexpr std::uint8_t r1 = 7;
static constexpr std::uint8_t l1 = 9;
static constexpr std::uint8_T r2 = 11;
#endif            // 32b/64b

template <std::uint8_t k>
static inline constexpr auto rotl(const t x) -> t {
  return (x << k) | (x >> (BITS - k));
}

auto next() -> t {
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
}

auto bit() -> bool {
  static t state;
  static std::uint8_t uses;
  if (uses == 0) {
    uses = BITS - 1;
    state = next();
  } else {
    --uses;
  }
  bool rtn = static_cast<bool>(state);  // === (state & 1)
  state >>= 1;
  return rtn;
}

}  // namespace rnd
