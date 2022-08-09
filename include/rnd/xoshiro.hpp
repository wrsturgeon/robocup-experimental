#pragma once

// Some edits to Blackman & Vigna's xoshiro PRNGs.
// See the below 64b & 32b sections for links.

#include <stdint.h>

namespace rnd {

#if BITS == 64       // https://prng.di.unimi.it/xoshiro256plusplus.c
using t = uint64_t;  // rnd::t
#elif BITS == 32     // https://prng.di.unimi.it/xoshiro128plusplus.c
using t = uint32_t;  // rnd::t
#endif               // 32b/64b

t next();

bool bit();

}  // namespace rnd
