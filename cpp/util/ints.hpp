#pragma once

#include <cstdint>

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
    static constexpr bool valid = true;                                                                                                                                                                                                                       \
    using t = UINT(bits);                                                                                                                                                                                                                                     \
  };                                                                                                                                                                                                                                                          \
  template <>                                                                                                                                                                                                                                                 \
  struct cint_s<bits, signed> {                                                                                                                                                                                                                               \
    static constexpr bool valid = true;                                                                                                                                                                                                                       \
    using t = INT(bits);                                                                                                                                                                                                                                      \
  }
// NOLINTEND(cppcoreguidelines-macro-usage)

MAKE_INT(8);
MAKE_INT(16);
MAKE_INT(32);
MAKE_INT(64);

#undef MAKE_INT
#undef INT
#undef UINT
#undef BLIND_INT
#undef BLIND_UINT

template <std::uint8_t bits, typename is_signed = unsigned>
requires ((bits >= 8) and (bits <= 64) and ((bits & (bits - 1)) == 0))  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
using cint = typename cint_s<bits, is_signed>::t;

template <typename T>
using full_t = cint<kSystemBits, T>;
using ifull_t = full_t<signed>;
using ufull_t = full_t<unsigned>;
