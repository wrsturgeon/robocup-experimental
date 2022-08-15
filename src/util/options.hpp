#pragma once

// Detects system architecture (32b/64b) and inform various algorithms
// accordingly. Compile with the compiler argument -D_BITS=<32|64> (whichever it
// is)
#ifdef _BITS

#if _BITS == 32 || _BITS == 64
static constexpr unsigned kSystemBits = _BITS;
#else
#error "Invalid value for _BITS"
#endif  // 32/64

#else
#error "Compile with argument -D_BITS=$(getconf LONG_BIT)"
#endif  // ifdef _BITS

#ifdef _IMAGE_H
static constexpr unsigned int kImageH = _IMAGE_H;
#else
static constexpr unsigned int kImageH = 960;
#endif

#ifdef _IMAGE_W
static constexpr unsigned int kImageW = _IMAGE_W;
#else
static constexpr unsigned int kImageW = 1280;
#endif

#define IMAGE_DIAG_SQ ((IMAGE_W * IMAGE_W) + (IMAGE_H * IMAGE_H))

#ifdef _NAO_HEIGHT_MM
static constexpr unsigned int kNaoHeightMM = _NAO_HEIGHT_MM;
#else
static constexpr unsigned int kNaoHeightMM = 500;  // TODO(wrsturgeon): this is a ROUGH
                                                   // ESTIMATE
#endif  // ifdef _NAO_HEIGHT_MM

#ifdef _DEBUG
// NOLINTNEXTLINE(modernize-use-bool-literals)
static constexpr auto kDebug = _DEBUG;
#else
static constexpr auto kDebug = false;
#endif  // ifdef _DEBUG
