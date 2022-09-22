#pragma once

#include <cstdint>

// Detects system architecture (32b/64b) and inform various algorithms
// accordingly. Compile with the compiler argument -DBITS=<32|64> (whichever it
// is)
#ifdef BITS

#if BITS == 32 || BITS == 64
static constexpr std::uint8_t kSystemBits = BITS;
#else
#error "Invalid value for BITS"
#endif  // 32/64

#else
#error "Compile with argument -DBITS=$(getconf LONG_BIT)"
#endif  // ifdef BITS

#ifdef IMAGE_H
static constexpr std::uint16_t kImageH = IMAGE_H;
#else
static constexpr std::uint16_t kImageH = 960;
#endif

#ifdef IMAGE_W
static constexpr std::uint16_t kImageW = IMAGE_W;
#else
static constexpr std::uint16_t kImageW = 1280;
#endif

#define IMAGE_DIAG_SQ ((IMAGE_W * IMAGE_W) + (IMAGE_H * IMAGE_H))

#ifdef NAO_HEIGHT_MM
static constexpr std::uint16_t kNaoHeightMM = NAO_HEIGHT_MM;
#else
static constexpr std::uint16_t kNaoHeightMM = 500;  // TODO(wrsturgeon): this is a ROUGH
                                                    // ESTIMATE
#endif  // ifdef NAO_HEIGHT_MM
