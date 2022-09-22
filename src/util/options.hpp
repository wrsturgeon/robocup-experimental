#pragma once

// Detects system architecture (32b/64b) and inform various algorithms
// accordingly. Compile with the compiler argument -DBITS=<32|64> (whichever it
// is)
#ifdef BITS

#if BITS == 32 || BITS == 64
static constexpr unsigned kSystemBits = BITS;
#else
#error "Invalid value for BITS"
#endif  // 32/64

#else
#error "Compile with argument -DBITS=$(getconf LONG_BIT)"
#endif  // ifdef BITS

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
