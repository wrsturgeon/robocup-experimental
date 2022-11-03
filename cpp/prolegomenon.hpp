#pragma once
// Manually included (via command-line argument) before any other files handed to the compiler

#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

// Compile with the command-line argument -DBITS=<system bits: 32 for Nao, usually 64 for PCs>
#ifdef BITS
#if BITS == 32 || BITS == 64
inline constexpr std::uint8_t kSystemBits = BITS;
#else
#error "Invalid value for BITS"
#endif  // 32/64
#else   // ifdef BITS
#error "Compile with argument -DBITS=$(getconf LONG_BIT)"
#endif  // ifdef BITS

/******** Custom integer types ********/

template <std::uint8_t bits, typename is_signed>
requires ((bits >= 8) and (bits <= 64) and ((bits & (bits - 1)) == 0))  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
struct cint_s {};

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define BLIND_UINT_T(bits) std::uint##bits##_t
#define BLIND_INT_T(bits) std::int##bits##_t
#define BLIND_UINT(bits) u##bits
#define BLIND_INT(bits) i##bits
#define UINT_T(bits) BLIND_UINT_T(bits)
#define INT_T(bits) BLIND_INT_T(bits)
#define UINT(bits) BLIND_UINT(bits)
#define INT(bits) BLIND_INT(bits)
#define MAKE_INT(bits)                                                                                                                                                                                                                                        \
  using INT(bits) = INT_T(bits);                                                                                                                                                                                                                              \
  using UINT(bits) = UINT_T(bits);                                                                                                                                                                                                                            \
  template <>                                                                                                                                                                                                                                                 \
  struct cint_s<bits, unsigned> {                                                                                                                                                                                                                             \
    using t = UINT(bits);                                                                                                                                                                                                                                     \
  };                                                                                                                                                                                                                                                          \
  template <>                                                                                                                                                                                                                                                 \
  struct cint_s<bits, signed> {                                                                                                                                                                                                                               \
    using t = INT(bits);                                                                                                                                                                                                                                      \
  }
MAKE_INT(8);
MAKE_INT(16);
MAKE_INT(32);
MAKE_INT(64);
#undef MAKE_INT
#undef INT
#undef UINT
#undef BLIND_INT
#undef BLIND_UINT
// NOLINTEND(cppcoreguidelines-macro-usage)

template <std::uint8_t bits, typename is_signed = unsigned>
requires ((bits >= 8) and (bits <= 64) and ((bits & (bits - 1)) == 0))  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
using cint = typename cint_s<bits, is_signed>::t;

template <typename T>
using full_t = cint<kSystemBits, T>;
using ifull_t = full_t<signed>;
using ufull_t = full_t<unsigned>;

/******** Utility `byte_ceil`: compile-time ceiling to 8, 16, 32, or 64 ********/

template <u8 n>
requires (n <= 64)                                                                        // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
inline constexpr u8 byte_ceil = (n <= 8) ? 8 : ((n <= 16) ? 16 : ((n <= 32) ? 32 : 64));  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

/******** Image size analysis and corresponding types ********/

#ifndef IMAGE_H
#define IMAGE_H 330
#endif
#ifndef IMAGE_W
#define IMAGE_W 750
#endif
static_assert(IMAGE_H > 0, "Negative image height");
static_assert(IMAGE_H <= 1024, "you think you aaaaare... cool goy? you think you a... zack markingberg? bigshoooooood now");
static_assert(IMAGE_W > 0, "Negative image width");
static_assert(IMAGE_W <= 1024, "nuh-uh");
using imsize_t = cint<byte_ceil<std::bit_width<ufull_t>((IMAGE_H > IMAGE_W) ? IMAGE_H : IMAGE_W)>, unsigned>;
inline constexpr imsize_t kImageH = IMAGE_H;
inline constexpr imsize_t kImageW = IMAGE_W;
#undef IMAGE_H
#undef IMAGE_W

using imsizediff_t = cint<byte_ceil<std::bit_width<ufull_t>((kImageH > kImageW) ? kImageH : kImageW) + 1>, signed>;
using px_t = std::pair<imsize_t, imsize_t>;
using pxdiff_t = std::pair<imsizediff_t, imsizediff_t>;

using imsquare_t = cint<byte_ceil<(std::bit_width<ufull_t>((kImageH > kImageW) ? kImageH : kImageW) << 1)>, unsigned>;
inline constexpr imsquare_t kImageArea = kImageH * kImageW;

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
#define CONST_IF_RELEASE const
#define NOX noexcept
#else
#define INLINE constexpr
#define impure [[nodiscard]]
#define NOX
#define CONST_IF_RELEASE
#endif  // NDEBUG
#define pure [[nodiscard]] INLINE

#ifndef TRIG_BITS
#define TRIG_BITS 16  // NOLINT(cppcoreguidelines-macro-usage)
#endif

#ifndef PYRAMID_FIND_THRESHOLD
#define PYRAMID_FIND_THRESHOLD 192  // NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
#endif
inline constexpr u8 kPyramidFindThreshold = PYRAMID_FIND_THRESHOLD;
#undef PYRAMID_FIND_THRESHOLD

// A gift from God
template <typename T>
pure auto
uninitialized() -> std::decay_t<T> {
  std::byte bytes[sizeof(T)];           // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  return *reinterpret_cast<T*>(bytes);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast,clang-analyzer-core.uninitialized.UndefReturn)
}
