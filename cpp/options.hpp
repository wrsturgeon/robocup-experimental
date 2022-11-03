#pragma once

#include <cstdint>

// Detects system architecture (32b/64b) and inform various algorithms
// accordingly. Compile with the compiler argument -DBITS=<32|64> (whichever it
// is)
#ifdef BITS

#if BITS == 32 || BITS == 64
inline constexpr std::uint8_t kSystemBits = BITS;
#else
#error "Invalid value for BITS"
#endif  // 32/64
// #undef BITS // not yet--for cint

#else
#error "Compile with argument -DBITS=$(getconf LONG_BIT)"
#endif  // ifdef BITS

using imsize_t = std::uint16_t;
using imsize_s_t = std::int16_t;
#ifdef IMAGE_H
inline constexpr imsize_t kImageH = IMAGE_H;
static_assert(kImageH == IMAGE_H);
#else
inline constexpr imsize_t kImageH = 330;
#endif
#undef IMAGE_H

#ifdef IMAGE_W
inline constexpr imsize_t kImageW = IMAGE_W;
static_assert(kImageW == IMAGE_W);
#else
inline constexpr imsize_t kImageW = 750;
#endif
#undef IMAGE_W

using imrdsq_t = std::uint32_t;
static_assert(sizeof(imrdsq_t) == (sizeof(imsize_t) << 1));  // for good measure
inline constexpr imrdsq_t kImageArea = kImageH * kImageW;

using nao_h_t = std::uint16_t;
#ifdef NAO_HEIGHT_MM
inline constexpr nao_h_t kNaoHeightMM = NAO_HEIGHT_MM;
#else
inline constexpr nao_h_t kNaoHeightMM = 550;  // TODO(wrsturgeon): ROUGH ESTIMATE
#endif  // ifdef NAO_HEIGHT_MM
#undef NAO_HEIGHT_MM

#ifdef NDEBUG
#define INLINE [[gnu::always_inline]] inline constexpr
#define impure [[nodiscard]] [[gnu::always_inline]] inline  // not constexpr since std::string for whatever reason isn't
#else
#define INLINE constexpr
#define impure [[nodiscard]]
#endif  // NDEBUG
#define pure [[nodiscard]] INLINE

#ifdef NDEBUG
#define CONST_IF_RELEASE const
#define NOX noexcept
#else
#define NOX
#define CONST_IF_RELEASE
#endif

#ifndef TRIG_BITS
#define TRIG_BITS 16  // NOLINT(cppcoreguidelines-macro-usage)
#endif
