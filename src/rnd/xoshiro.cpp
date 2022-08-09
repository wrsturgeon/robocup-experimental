#include "rnd/xoshiro.hpp"

namespace rnd {

template <uint8_t k>
static inline constexpr auto rotl(const t x) -> t {
  return (x << k) | (x >> (BITS - k));
}

#if BITS == 64
// https://prng.di.unimi.it/xoshiro256plusplus.c

static t s[4] = {0xe220a8397b1dcdaf, 0x6e789e6aa1b965f4, 0x6c45d188009454f, 0xf88bb8a8724c81ec}; // global

auto next() -> t {
  t const result = rotl<23>(s[0] + s[3]) + s[0];

  t const tmp = s[1] << 17;

  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];

  s[2] ^= tmp;

  s[3] = rotl<45>(s[3]);

  return result;
}

#elif BITS == 32
// https://prng.di.unimi.it/xoshiro128plusplus.c

static t s[4] = {0x7b1dcdaf, 0xa1b965f4, 0x8009454f, 0x724c81ec}; // global

auto next() -> t {
  const t result = rotl<7>(s[0] + s[3]) + s[0];

  const t t = s[1] << 9;

  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];

  s[2] ^= t;

  s[3] = rotl<11>(s[3]);

  return result;
}

#endif // 32b/64b

auto bit() -> bool {
  static t state;
  static uint8_t uses;
  if (!uses) {
    uses = BITS - 1;
    state = next();
  } else {
    --uses;
  }
  bool rtn = state & 1;
  state >>= 1;
  return rtn;
}

} // namespace rnd
