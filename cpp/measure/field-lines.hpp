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

inline constexpr auto kCenter = static_cast<mm_t>(fp::from_int(0));
inline constexpr auto kHEdge = static_cast<mm_t>(fp::from_int(4500));
inline constexpr auto kVEdge = static_cast<mm_t>(fp::from_int(3000));
inline constexpr auto kHGoal = static_cast<mm_t>(fp::from_int(3900));
inline constexpr auto kVGoal = static_cast<mm_t>(fp::from_int(1100));
inline constexpr auto kHPen = static_cast<mm_t>(fp::from_int(2850));
inline constexpr auto kVPen = static_cast<mm_t>(fp::from_int(2000));

inline constexpr mm_t kLEdge = -kHEdge;
inline constexpr mm_t kREdge = kHEdge;
inline constexpr mm_t kTEdge = -kVEdge;
inline constexpr mm_t kBEdge = kVEdge;
inline constexpr mm_t kLGoal = -kHGoal;
inline constexpr mm_t kRGoal = kHGoal;
inline constexpr mm_t kTGoal = -kVGoal;
inline constexpr mm_t kBGoal = kVGoal;
inline constexpr mm_t kLPen = -kHPen;
inline constexpr mm_t kRPen = kHPen;
inline constexpr mm_t kTPen = -kVPen;
inline constexpr mm_t kBPen = kVPen;

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
        return (x < 6000) ? xw_t{kLEdge, fp::from_int(x - 3000), zero} : xw_t{fp::from_int(x - 10500), kBEdge, zero};
      }
      return (x < 21000) ? xw_t{kREdge, fp::from_int(x - 18000), zero} : xw_t{fp::from_int(x - 25500), kTEdge, zero};
    }
    if (x < 48950) {
      if (x < 37650) {
        return (x < 36000) ? xw_t{kCenter, fp::from_int(x - 33000), zero} : xw_t{fp::from_int(x - 40500), kTPen, zero};
      }
      if (x < 43300) {
        return (x < 41650) ? xw_t{kLPen, fp::from_int(x - 39650), zero} : xw_t{fp::from_int(x - 46150), kBPen, zero};
      }
      return (x < 44950) ? xw_t{fp::from_int(x - 40450), kTPen, zero} : xw_t{kRPen, fp::from_int(x - 46950), zero};
    }
    if (x < 54000) {
      if (x < 51200) {
        return (x < 50600) ? xw_t{fp::from_int(x - 46100), kBPen, zero} : xw_t{fp::from_int(x - 55100), kTGoal, zero};
      }
      return (x < 53400) ? xw_t{kLGoal, fp::from_int(x - 52300), zero} : xw_t{fp::from_int(x - 57900), kBGoal, zero};
    }
    if (x < 56800) {
      return (x < 54600) ? xw_t{fp::from_int(x - 50100), kTGoal, zero} : xw_t{kRGoal, fp::from_int(x - 55700), zero};
    }
    if (x < 57400) { return xw_t{fp::from_int(x - 52900), kBGoal, zero}; }
    // TODO(wrsturgeon): consider, instead of resampling, sampling green
    // TODO(wrsturgeon): no, sample the fucking GOALPOSTS!!!
  } while (true);  // If >= 57400, resample
}

// NOLINTEND(clang-diagnostic-sign-conversion)
// NOLINTEND(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

}  // namespace measure
