#pragma once

#include "util/ints.hpp"

#include <bit>

#ifndef NDEBUG
#include <stdexcept>
#endif

// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
template <u8 n> requires (n <= 64)  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static constexpr u8 byte_ceil = (n <= 8) ? 8 : ((n <= 16) ? 16 : ((n <= 32) ? 32 : 64));
