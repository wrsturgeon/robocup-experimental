#pragma once
// Manually included (via command-line argument) before any other files handed to the compiler

#include <bit>
#include <cstddef>
#include <cstdint>
#include <tuple>
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
#if BITS >= 8
MAKE_INT(8);
#if BITS >= 16
MAKE_INT(16);
#if BITS >= 32
MAKE_INT(32);
#if BITS >= 64
MAKE_INT(64);
#endif  // BITS >= 64
#endif  // BITS >= 32
#endif  // BITS >= 16
#endif  // BITS >= 8
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
#define IMAGE_H 330  // NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
#endif
#ifndef IMAGE_W
#define IMAGE_W 750  // NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
#endif
static_assert(IMAGE_H > 0, "Negative image height");
static_assert(IMAGE_H <= 1024, "you think you aaaaare... cool goy? you think you a... zack markingberg? bigshoooooood now");  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert(IMAGE_W > 0, "Negative image width");
static_assert(IMAGE_W <= 1024, "nuh-uh");  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
using imsize_t = cint<byte_ceil<std::bit_width<ufull_t>((IMAGE_H > IMAGE_W) ? IMAGE_H : IMAGE_W)>, unsigned>;
inline constexpr imsize_t kImageH = IMAGE_H;
inline constexpr imsize_t kImageW = IMAGE_W;
#undef IMAGE_H
#undef IMAGE_W

inline constexpr u8 pxidx_bits = std::bit_width<ufull_t>(((kImageH > kImageW) ? kImageH : kImageW) - 1);
using pxidx_t = cint<byte_ceil<pxidx_bits>, unsigned>;
using pxidxdiff_t = cint<byte_ceil<pxidx_bits + 1>, signed>;
using px_t = std::pair<pxidx_t, pxidx_t>;
using pxdiff_t = std::pair<pxidxdiff_t, pxidxdiff_t>;

using imsquare_t = cint<byte_ceil<(std::bit_width<ufull_t>((kImageH > kImageW) ? kImageH : kImageW) << 1)>, unsigned>;
inline constexpr imsquare_t kImageArea = kImageH * kImageW;

/******** Real-world measurement, similar to the above ********/

#ifndef FIELD_H
#define FIELD_H 6000  // NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
#endif
#ifndef FIELD_W
#define FIELD_W 9000  // NOLINT(cppcoreguidelines-macro-usage,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
#endif
static_assert(FIELD_H > 0, "Negative field height");
static_assert(FIELD_W > 0, "Negative field width");
using fieldsize_t = cint<byte_ceil<std::bit_width<ufull_t>((FIELD_H > FIELD_W) ? FIELD_H : FIELD_W)>, unsigned>;
inline constexpr fieldsize_t kFieldH = FIELD_H;  // millimeters
inline constexpr fieldsize_t kFieldW = FIELD_W;  // millimeters
#undef FIELD_H
#undef FIELD_W
inline constexpr u8 mm_bits = std::bit_width<ufull_t>(((kFieldH > kFieldW) ? kFieldH : kFieldW) - 1);
// using mmidx_t = cint<byte_ceil<mm_bits>, unsigned>;
// using mmidxdiff_t = cint<byte_ceil<mm_bits + 1>, signed>;
// using mm_t = std::pair<mmidx_t, mmidx_t>;
// using mmdiff_t = std::pair<mmidxdiff_t, mmidxdiff_t>;
using mm_t = cint<byte_ceil<mm_bits + 1>, signed>;

using xw_t = std::tuple<mm_t, mm_t, mm_t>;
using xp_t = std::tuple<pxidx_t, pxidx_t, mm_t>;

using nao_h_t = std::uint16_t;
#ifdef NAO_HEIGHT_MM
inline constexpr nao_h_t kNaoHeightMM = NAO_HEIGHT_MM;
#else
inline constexpr nao_h_t kNaoHeightMM = 550;  // TODO(wrsturgeon): ROUGH ESTIMATE
#endif  // ifdef NAO_HEIGHT_MM
#undef NAO_HEIGHT_MM

/******** Useful macros ********/

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

/******** A gift from God ********/

template <typename T>
pure auto
uninitialized() -> std::decay_t<T> {
  std::byte bytes[sizeof(T)];           // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  return *reinterpret_cast<T*>(bytes);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast,clang-analyzer-core.uninitialized.UndefReturn)
}

/******** Eigen types ********/

template <int H, int W>
inline constexpr auto RowMajor = ((W == 1) ? Eigen::ColMajor : Eigen::RowMajor);
template <int H, int W>
inline constexpr auto ColMajor = ((H == 1) ? Eigen::RowMajor : Eigen::ColMajor);

template <int H, int W, typename T = u8>
requires ((W > 0) and (H > 0))
using Array = Eigen::Array<T, H, W, RowMajor<H, W>>;
template <int N, typename T = u8>
using Vector = Array<N, 1, T>;  // Column vector

template <int H, int W, int C, typename T = u8>
using Tensor = Eigen::TensorFixedSize<T, Eigen::Sizes<H, W, C>, Eigen::RowMajor>;
template <int H = kImageH, int W = kImageW, int C = 3, typename T = u8>
using ImageTensor = Tensor<H, W, C, T>;
template <int H = kImageH, int W = kImageW, int C = 3, typename T = u8>
requires (C > 1)
using ImageMap = Eigen::Map<Array<H * W, C, T>>;
template <int H = kImageH, int W = kImageW, typename T = u8>
using ChannelMap = Eigen::Map<Array<H, W, T>, RowMajor<H, W>>;
