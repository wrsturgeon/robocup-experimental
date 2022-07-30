#ifndef RND_XOSHIRO_HPP_
#define RND_XOSHIRO_HPP_

// Some edits to Blackman & Vigna's xoshiro PRNGs.
// See the below 64b & 32b sections for links.

#include <stdint.h>

namespace rnd {

#if BITS == 64 // https://prng.di.unimi.it/xoshiro256plusplus.c
using t = uint64_t; // rnd::t
#elif BITS == 32 // https://prng.di.unimi.it/xoshiro128plusplus.c
using t = uint32_t; // rnd::t
#endif // 32b/64b

static t s[4];

template <uint8_t k>
INLINE constexpr t rotl(t const x);

INLINE t next();

INLINE bool bit();



} // namespace rnd

#endif // RND_XOSHIRO_HPP_
