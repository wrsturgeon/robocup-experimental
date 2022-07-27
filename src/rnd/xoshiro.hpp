#if RND_ENABLED
#ifndef RND_XOSHIRO_HPP_
#define RND_XOSHIRO_HPP_

// Some edits to Blackman & Vigna's xoshiro PRNGs.

#include <stdint.h>

namespace rnd {



#if BITS == 64

using t = uint64_t; // rnd::t

// https://prng.di.unimi.it/xoshiro256plusplus.c

/*  Written in 2019 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

template <uint8_t k>
INLINE constexpr rnd::t rotl(const rnd::t x) { return (x << k) | (x >> (64 - k)); }

static rnd::t s[4] = {0xe220a8397b1dcdaf, 0x6e789e6aa1b965f4, 0x6c45d188009454f, 0xf88bb8a8724c81ec};

INLINE rnd::t next() {
	const rnd::t result = rotl<23>(s[0] + s[3]) + s[0];

	const rnd::t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl<45>(s[3]);

	return result;
}



#elif BITS == 32

using t = uint32_t; // rnd::t

// https://prng.di.unimi.it/xoshiro128plusplus.c

/*  Written in 2019 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

template <uint8_t k>
INLINE constexpr rnd::t rotl(const rnd::t x) { return (x << k) | (x >> (32 - k)); }

static rnd::t s[4] = {0x7b1dcdaf, 0xa1b965f4, 0x8009454f, 0x724c81ec};

INLINE rnd::t next() {
	const rnd::t result = rotl<7>(s[0] + s[3]) + s[0];

	const rnd::t t = s[1] << 9;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl<11>(s[3]);

	return result;
}



#endif // 32b/64b



static uint8_t bit() {
  static rnd::t state;
  static uint8_t uses;
  if (!uses) {
    uses = BITS - 1;
    state = rnd::next();
  } else { --uses; }
  uint8_t rtn = state & 1;
  state >>= 1;
  return rtn;
}



} // namespace rnd

#endif // RND_XOSHIRO_HPP_

#else // RND_ENABLED
#pragma message("Skipping xoshiro.hpp; rnd module disabled")
#endif // RND_ENABLED
