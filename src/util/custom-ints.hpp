#pragma once

#include <cstdint>

template <std::uint8_t bits> struct custom_int;

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define BLIND_UINT(bits) std::uint##bits##_t
#define BLIND_INT(bits) std::int##bits##_t
#define UINT(bits) BLIND_UINT(bits)
#define INT(bits) BLIND_INT(bits)
#define MAKE_INT(bits)                  \
  template <> struct custom_int<bits> { \
    using signed_t = INT(bits);         \
    using unsigned_t = UINT(bits);      \
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
