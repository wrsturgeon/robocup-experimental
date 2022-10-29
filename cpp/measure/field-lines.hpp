#pragma once

#include "rnd/xoshiro.hpp"

#include "fp/fixed-point.hpp"
#include "measure/units.hpp"
#include "util/ints.hpp"
#include "util/units.hpp"

#include <cstdint>

namespace measure {

/**
 * Table of values from the above lines as written
 *
 *  indx        x             y         +=     cum
 *
 *  %%%%%%%%%%%%%%%%%% Outer rectangle
 *  [ 1]      -4500      -3000 +3000   6000    6000
 *  [ 2]   -4500 +4500      +3000      9000   15000
 *  [ 3]      +4500      -3000 +3000   6000   21000
 *  [ 4]   -4500 +4500      -3000      9000   30000
 *
 *  %%%%%%%%%%%%%%%%%% Center line
 *  [ 5]      +   0      -3000 +3000   6000   36000
 *
 *  %%%%%%%%%%%%%%%%%% Penalty area left
 *  [ 6]   -4500 -2850      -2000      1650   37650
 *  [ 7]      -2850      -2000 +2000   4000   41650
 *  [ 8]   -4500 -2850      +2000      1650   43300
 *
 *  %%%%%%%%%%%%%%%%%% Penalty area right
 *  [ 9]   +2850 +4500      -2000      1650   44950
 *  [10]      +2850      -2000 +2000   4000   48950
 *  [11]   +2850 +4500      +2000      1650   50600
 *
 *  %%%%%%%%%%%%%%%%%% Goal area left
 *  [12]   -4500 -3900      -1100       600   51200
 *  [13]      -3900      -1100 +1100   2200   53400
 *  [14]   -4500 -3900      +1100       600   54000
 *
 *  %%%%%%%%%%%%%%%%%% Goal area right
 *  [15]   +3900 +4500      -1100       600   54600
 *  [16]      +3900      -1100 +1100   2200   56800
 *  [17]   +3900 +4500      +1100       600   57400
 */

inline constexpr auto kCenter = mm_t{fp::from_int(0)};
inline constexpr auto kHEdge = mm_t{fp::from_int(4500)};
inline constexpr auto kVEdge = mm_t{fp::from_int(3000)};
inline constexpr auto kHGoal = mm_t{fp::from_int(3900)};
inline constexpr auto kVGoal = mm_t{fp::from_int(1100)};
inline constexpr auto kHPen = mm_t{fp::from_int(2850)};
inline constexpr auto kVPen = mm_t{fp::from_int(2000)};
inline constexpr auto kHPost = mm_t{fp::from_int(800)};
inline constexpr auto kWPost = mm_t{fp::from_int(750)};

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
// NOLINTBEGIN(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
// NOLINTBEGIN(clang-diagnostic-sign-conversion)

[[nodiscard]] inline static auto
sample_field_lines() -> xw_t {
  static constexpr auto zero = mm_t::zero();
  static constexpr u8 rnd_uses = (kSystemBits >> 4);
  static rnd::t rnd_state;
  static u8 rnd_uses_left;
  do {
    if (rnd_uses_left == 0) {
      rnd_uses_left = rnd_uses - 1;
      rnd_state = rnd::next();
    } else {
      --rnd_uses_left;
    }
    auto x = static_cast<u16>(rnd_state);
    rnd_state >>= 16;
    if (x < 30000) {
      if (x < 15000) {
        return (x < 6000) ? xw_t{-kHEdge, fp::from_int(x - 3000), zero} : xw_t{fp::from_int(x - 10500), kVEdge, zero};
      }
      return (x < 21000) ? xw_t{kHEdge, fp::from_int(x - 18000), zero} : xw_t{fp::from_int(x - 25500), -kVEdge, zero};
    }
    if (x < 48950) {
      if (x < 37650) {
        return (x < 36000) ? xw_t{kCenter, fp::from_int(x - 33000), zero} : xw_t{fp::from_int(x - 40500), -kVPen, zero};
      }
      if (x < 43300) {
        return (x < 41650) ? xw_t{-kHPen, fp::from_int(x - 39650), zero} : xw_t{fp::from_int(x - 46150), kVPen, zero};
      }
      return (x < 44950) ? xw_t{fp::from_int(x - 40450), -kVPen, zero} : xw_t{kHPen, fp::from_int(x - 46950), zero};
    }
    if (x < 54000) {
      if (x < 51200) {
        return (x < 50600) ? xw_t{fp::from_int(x - 46100), kVPen, zero} : xw_t{fp::from_int(x - 55100), -kVGoal, zero};
      }
      return (x < 53400) ? xw_t{-kHGoal, fp::from_int(x - 52300), zero} : xw_t{fp::from_int(x - 57900), kVGoal, zero};
    }
    if (x < 56800) {
      return (x < 54600) ? xw_t{fp::from_int(x - 50100), -kVGoal, zero} : xw_t{kHGoal, fp::from_int(x - 55700), zero};
    }
    if (x < 57400) { return xw_t{fp::from_int(x - 52900), kVGoal, zero}; }
    if (x < 57400 + kHPost.round()) { return xw_t{-kHEdge, -kWPost, fp::from_int(x - 57400)}; }
    if (x < 57400 + kHPost.round() + (kWPost.round() << 1)) {
      return xw_t{-kHEdge, fp::from_int(x - 57400 - kHPost.round() - kWPost.round()), kHPost};
    }
    static constexpr auto cutoff = 57400 + ((kHPost.round() + kWPost.round()) << 1);
    if (x < cutoff) { return xw_t{-kHEdge, kWPost, fp::from_int(x - 57400 - (kHPost.round() << 1) - kWPost.round())}; }
    if (x < cutoff + kHPost.round()) { return xw_t{kHEdge, -kWPost, fp::from_int(x - cutoff)}; }
    if (x < cutoff + kHPost.round() + (kWPost.round() << 1)) {
      return xw_t{kHEdge, fp::from_int(x - cutoff - kHPost.round() - kWPost.round()), kHPost};
    }
    if (x < cutoff + ((kHPost.round() + kWPost.round()) << 1)) {
      return xw_t{kHEdge, kWPost, fp::from_int(x - cutoff - (kHPost.round() << 1) - kWPost.round())};
    }
  } while (true);  // If >= 63600, resample
}

// NOLINTEND(clang-diagnostic-sign-conversion)
// NOLINTEND(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

}  // namespace measure
