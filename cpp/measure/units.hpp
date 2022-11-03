#pragma once

#include "fp/fixed-point.hpp"

namespace measure {

static constexpr auto kFieldWidth = 9000;   // millimeters
static constexpr auto kFieldHeight = 6000;  // millimeters
static constexpr auto kFieldDiag = isqrt<imsize_t>((kFieldWidth * kFieldWidth) + (kFieldHeight * kFieldHeight));
static constexpr auto kFieldDiagBits = static_cast<u8>(std::bit_width(std::bit_ceil(kFieldDiag)));
using mm_t = fp::t<kSystemBits, kFieldDiagBits, signed>;
using xw_t = fp::a<3, kSystemBits, kFieldDiagBits, signed>;
using mm_sq_t = fp::t<kSystemBits, (kFieldDiagBits << 1), signed>;
static_assert(kFieldDiagBits < 16);
using mm16_t = fp::t<16, kFieldDiagBits, signed>;
using xw16_t = fp::a<3, 16, kFieldDiagBits, signed>;

}  // namespace measure
