#pragma once

// Some edits to Blackman & Vigna's xoshiro PRNGs.
// See the below 64b & 32b sections for links.

#include <array>
#include <cstdint>

namespace rnd {

#if BITS == 64
using t = std::uint64_t;  // rnd::t
#elif BITS == 32
using t = std::uint32_t;  // rnd::t
#endif  // 32b/64b

auto next() -> t;

auto bit() -> bool;

}  // namespace rnd
