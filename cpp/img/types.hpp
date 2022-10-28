#pragma once

#include "fp/fixed-point.hpp"
#include "util/ints.hpp"

#include <array>
#include <bit>

namespace img {

inline constexpr auto diag = isqrt<imsize_t>((kImageH * kImageH) + (kImageW * kImageW));
inline constexpr auto lg_diag = static_cast<u8>(std::bit_width(std::bit_ceil(diag)));
inline constexpr auto lg_diag_byte_u = byte_ceil(lg_diag);
inline constexpr auto lg_diag_byte_s = byte_ceil(lg_diag + 1);

template <u8 B = lg_diag_byte_u> requires (B >= lg_diag)
using px_t = fp::t<B, lg_diag, unsigned>;
template <u8 B = lg_diag_byte_s> requires (B > lg_diag)
using pxdiff_t = fp::t<B, lg_diag, signed>;
template <u8 B = lg_diag_byte_u> requires (B >= lg_diag)
using idx_t = fp::a<2, B, lg_diag, unsigned>;
template <u8 B = lg_diag_byte_s> requires (B > lg_diag)
using idxdiff_t = fp::a<2, B, lg_diag, signed>;
using pxint_t = cint<lg_diag_byte_u, unsigned>;
using pxintdiff_t = cint<lg_diag_byte_s, signed>;
using idxint_t = std::array<pxint_t, 2>;
using idxintdiff_t = std::array<pxintdiff_t, 2>;

}  // namespace img
