#ifndef RND_XOSHIRO_HPP_
#define RND_XOSHIRO_HPP_

// Some edits to Blackman & Vigna's xoshiro PRNGs.
// See the below 64b & 32b sections for links.

#include <stdint.h>

namespace rnd {

#if BITS == 64 // https://prng.di.unimi.it/xoshiro256plusplus.c
using t = uint64_t; // rnd::t
static t s[4] = {0xe220a8397b1dcdaf, 0x6e789e6aa1b965f4, 0x6c45d188009454f, 0xf88bb8a8724c81ec}; // global
#elif BITS == 32 // https://prng.di.unimi.it/xoshiro128plusplus.c
using t = uint32_t; // rnd::t
static t s[4] = {0x7b1dcdaf, 0xa1b965f4, 0x8009454f, 0x724c81ec}; // global
#endif // 32b/64b

template <uint8_t k>
static inline constexpr t rotl(t const x);

t next();

bool bit();



} // namespace rnd

#endif // RND_XOSHIRO_HPP_
