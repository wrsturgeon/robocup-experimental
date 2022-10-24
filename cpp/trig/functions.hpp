#pragma once

#include "fp/fixed-point.hpp"
#include "util/ints.hpp"
#include "util/ternary.hpp"

#include <array>

namespace trig {

namespace lookup {
using t = cint<TRIG_BITS, signed>;
// clang-format off
#if TRIG_BITS == 8
inline constexpr std::array<t, 256> sin = {0, 3, 6, 9, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 52, 55, 58, 60, 63, 66, 68, 71, 74, 76, 79, 81, 84, 86, 88, 91, 93, 95, 97, 99, 101, 103, 105, 106, 108, 110, 111, 113, 114, 116, 117, 118, 119, 121, 122, 122, 123, 124, 125, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 125, 124, 123, 122, 122, 121, 119, 118, 117, 116, 114, 113, 111, 110, 108, 106, 105, 103, 101, 99, 97, 95, 93, 91, 88, 86, 84, 81, 79, 76, 74, 71, 68, 66, 63, 60, 58, 55, 52, 49, 46, 43, 40, 37, 34, 31, 28, 25, 22, 19, 16, 13, 9, 6, 3, 0, -3, -6, -9, -13, -16, -19, -22, -25, -28, -31, -34, -37, -40, -43, -46, -49, -52, -55, -58, -60, -63, -66, -68, -71, -74, -76, -79, -81, -84, -86, -88, -91, -93, -95, -97, -99, -101, -103, -105, -106, -108, -110, -111, -113, -114, -116, -117, -118, -119, -121, -122, -122, -123, -124, -125, -126, -126, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -126, -126, -125, -124, -123, -122, -122, -121, -119, -118, -117, -116, -114, -113, -111, -110, -108, -106, -105, -103, -101, -99, -97, -95, -93, -91, -88, -86, -84, -81, -79, -76, -74, -71, -68, -66, -63, -60, -58, -55, -52, -49, -46, -43, -40, -37, -34, -31, -28, -25, -22, -19, -16, -13, -9, -6, -3};
inline constexpr std::array<t, 256> cos = {127, 127, 127, 127, 127, 127, 127, 126, 126, 125, 124, 123, 122, 122, 121, 119, 118, 117, 116, 114, 113, 111, 110, 108, 106, 105, 103, 101, 99, 97, 95, 93, 91, 88, 86, 84, 81, 79, 76, 74, 71, 68, 66, 63, 60, 58, 55, 52, 49, 46, 43, 40, 37, 34, 31, 28, 25, 22, 19, 16, 13, 9, 6, 3, 0, -3, -6, -9, -13, -16, -19, -22, -25, -28, -31, -34, -37, -40, -43, -46, -49, -52, -55, -58, -60, -63, -66, -68, -71, -74, -76, -79, -81, -84, -86, -88, -91, -93, -95, -97, -99, -101, -103, -105, -106, -108, -110, -111, -113, -114, -116, -117, -118, -119, -121, -122, -122, -123, -124, -125, -126, -126, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -126, -126, -125, -124, -123, -122, -122, -121, -119, -118, -117, -116, -114, -113, -111, -110, -108, -106, -105, -103, -101, -99, -97, -95, -93, -91, -88, -86, -84, -81, -79, -76, -74, -71, -68, -66, -63, -60, -58, -55, -52, -49, -46, -43, -40, -37, -34, -31, -28, -25, -22, -19, -16, -13, -9, -6, -3, 0, 3, 6, 9, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 52, 55, 58, 60, 63, 66, 68, 71, 74, 76, 79, 81, 84, 86, 88, 91, 93, 95, 97, 99, 101, 103, 105, 106, 108, 110, 111, 113, 114, 116, 117, 118, 119, 121, 122, 122, 123, 124, 125, 126, 126, 127, 127, 127, 127, 127, 127};
#else // TRIG_BITS
#error "Chosen value for TRIG_BITS unimplemented"
#endif // TRIG_BITS
// clang-format on
}  // namespace lookup

using rtn_t = fp::t<TRIG_BITS, 0, signed>;
using rtn_array_t = fp::a<2, TRIG_BITS, 0, signed>;

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MAKE_TRIG_FN(NAME, RTNTYPE, ...)                                                                                      \
  pure auto NAME(u8 const x) noexcept -> RTNTYPE { return RTNTYPE{__VA_ARGS__}; }                                             \
  template <FixedPoint T> pure auto NAME(T const x) noexcept -> RTNTYPE {                                                     \
    if constexpr (T::f < 8) { return NAME(static_cast<u8>((+x) << (8 - T::f))); }                                             \
    return NAME(static_cast<u8>((+x) >> (T::f - 8)));                                                                         \
  }
MAKE_TRIG_FN(cos, rtn_t, lookup::cos[x])
MAKE_TRIG_FN(sin, rtn_t, lookup::sin[x])
MAKE_TRIG_FN(exp, rtn_array_t, rtn_t{lookup::cos[x]}, rtn_t{lookup::sin[x]})

}  // namespace trig
