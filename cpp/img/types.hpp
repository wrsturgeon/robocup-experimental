#pragma once

#include "fp/fixed-point.hpp"
#include "util/ints.hpp"

#include <bit>

namespace img {

using imsize_t = u16;

inline constexpr auto kImageDiag = isqrt<imsize_t>((kImageH * kImageH) + (kImageW * kImageW));
inline constexpr auto kLgImageDiag = static_cast<u8>(std::bit_width(std::bit_ceil(kImageDiag)));

// We need to be able to represent the negative image diagonal to subtract any two points
inline constexpr u8 kPxTBits = kLgImageDiag + 1;
inline constexpr u8 kPxTPw2 = byte_ceil(kPxTBits);  // e.g. 11 -> 16

using pxint_t = cint<kPxTPw2, signed>;
using px_t = fp::t<kPxTPw2, 2, signed>;  // wiggle room for severe distortion
using px2d = fp::a<2, kPxTPw2, 2, signed>;

}  // namespace img
