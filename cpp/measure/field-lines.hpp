#pragma once

#include "rnd/xoshiro.hpp"

#include "fp/fixed-point.hpp"
#include "util/units.hpp"

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

inline constexpr mm_t kCenter = 0;
inline constexpr mm_t kHEdge = 4500;
inline constexpr mm_t kVEdge = 3000;
inline constexpr mm_t kHGoal = 3900;
inline constexpr mm_t kVGoal = 1100;
inline constexpr mm_t kHPen = 2850;
inline constexpr mm_t kVPen = 2000;
inline constexpr mm_t kHPost = 800;
inline constexpr mm_t kWPost = 750;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
// NOLINTBEGIN(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
// NOLINTBEGIN(clang-diagnostic-sign-conversion)

[[nodiscard]] inline static auto
sample_field_lines() -> xw_t {
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
      if (x < 15000) { return (x < 6000) ? xw_t{-kHEdge, x - 3000, 0} : xw_t{x - 10500, kVEdge, 0}; }
      return (x < 21000) ? xw_t{kHEdge, x - 18000, 0} : xw_t{x - 25500, -kVEdge, 0};
    }
    if (x < 48950) {
      if (x < 37650) { return (x < 36000) ? xw_t{kCenter, x - 33000, 0} : xw_t{x - 40500, -kVPen, 0}; }
      if (x < 43300) { return (x < 41650) ? xw_t{-kHPen, x - 39650, 0} : xw_t{x - 46150, kVPen, 0}; }
      return (x < 44950) ? xw_t{x - 40450, -kVPen, 0} : xw_t{kHPen, x - 46950, 0};
    }
    if (x < 54000) {
      if (x < 51200) { return (x < 50600) ? xw_t{x - 46100, kVPen, 0} : xw_t{x - 55100, -kVGoal, 0}; }
      return (x < 53400) ? xw_t{-kHGoal, x - 52300, 0} : xw_t{x - 57900, kVGoal, 0};
    }
    if (x < 56800) { return (x < 54600) ? xw_t{x - 50100, -kVGoal, 0} : xw_t{kHGoal, x - 55700, 0}; }
    if (x < 57400) { return xw_t{x - 52900, kVGoal, 0}; }
    if (x < 57400 + kHPost) { return xw_t{-kHEdge, -kWPost, x - 57400}; }
    if (x < 57400 + kHPost + (kWPost << 1)) { return xw_t{-kHEdge, x - 57400 - kHPost - kWPost, kHPost}; }
    static constexpr auto cutoff = 57400 + ((kHPost + kWPost) << 1);
    if (x < cutoff) { return xw_t{-kHEdge, kWPost, x - 57400 - (kHPost << 1) - kWPost}; }
    if (x < cutoff + kHPost) { return xw_t{kHEdge, -kWPost, x - cutoff}; }
    if (x < cutoff + kHPost + (kWPost << 1)) { return xw_t{kHEdge, x - cutoff - kHPost - kWPost, kHPost}; }
    if (x < cutoff + ((kHPost + kWPost) << 1)) { return xw_t{kHEdge, kWPost, x - cutoff - (kHPost << 1) - kWPost}; }
  } while (true);  // If >= 63600, resample
}

// NOLINTEND(clang-diagnostic-sign-conversion)
// NOLINTEND(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

}  // namespace measure
