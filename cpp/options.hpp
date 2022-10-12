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

#ifdef IMAGE_H
inline constexpr std::uint16_t kImageH = IMAGE_H;
#else
inline constexpr std::uint16_t kImageH = 330;
#endif
#undef IMAGE_H

#ifdef IMAGE_W
inline constexpr std::uint16_t kImageW = IMAGE_W;
#else
inline constexpr std::uint16_t kImageW = 750;
#endif
#undef IMAGE_W

inline constexpr std::uint32_t kImageArea = kImageH * kImageW;

#ifdef NAO_HEIGHT_MM
inline constexpr std::uint16_t kNaoHeightMM = NAO_HEIGHT_MM;
#else
inline constexpr std::uint16_t kNaoHeightMM = 500;  // TODO(wrsturgeon): ROUGH ESTIMATE
#endif  // ifdef NAO_HEIGHT_MM
#undef NAO_HEIGHT_MM

#define INLINE [[gnu::always_inline]] inline constexpr
#define pure [[nodiscard]] INLINE

#ifdef NDEBUG
#define NOX noexcept
#else
#define NOX
#endif
