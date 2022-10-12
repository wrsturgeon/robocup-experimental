#pragma once

#include "rnd/xoshiro.hpp"

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

inline constexpr i16 kCenter = 0;
inline constexpr i16 kHEdge = 4500;
inline constexpr i16 kVEdge = 3000;
inline constexpr i16 kHGoal = 3900;
inline constexpr i16 kVGoal = 1100;
inline constexpr i16 kHPen = 2850;
inline constexpr i16 kVPen = 2000;

inline constexpr i16 kLEdge = -kHEdge;
inline constexpr i16 kREdge = kHEdge;
inline constexpr i16 kTEdge = -kVEdge;
inline constexpr i16 kBEdge = kVEdge;
inline constexpr i16 kLGoal = -kHGoal;
inline constexpr i16 kRGoal = kHGoal;
inline constexpr i16 kTGoal = -kVGoal;
inline constexpr i16 kBGoal = kVGoal;
inline constexpr i16 kLPen = -kHPen;
inline constexpr i16 kRPen = kHPen;
inline constexpr i16 kTPen = -kVPen;
inline constexpr i16 kBPen = kVPen;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
// NOLINTBEGIN(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
// NOLINTBEGIN(clang-diagnostic-sign-conversion)

// [[nodiscard]] static auto sample_field_lines() -> ds2d {
//   static constexpr u8 rnd_uses = (kSystemBits >> 4);
//   static rnd::t rnd_state;
//   static u8 rnd_uses_left;
//   do {
//     if (!rnd_uses_left) {
//       rnd_uses_left = rnd_uses - 1;
//       rnd_state = rnd::next();
//     } else {
//       --rnd_uses_left;
//     }
//     auto x = static_cast<u16>(rnd_state);
//     rnd_state >>= 16;
//     if (x < 30000) {
//       if (x < 15000) {
//         return (x < 6000)
//               ? ds2d{kLEdge, static_cast<i16>(x - 3000)}
//               : ds2d{static_cast<i16>(x - 10500), kBEdge};
//       }
//       return (x < 21000)
//             ? ds2d{kREdge, static_cast<i16>(x - 18000)}
//             : ds2d{static_cast<i16>(x - 25500), kTEdge};
//     }
//     if (x < 48950) {
//       if (x < 37650) {
//         return (x < 36000)
//               ? ds2d{kCenter, static_cast<i16>(x - 33000)}
//               : ds2d{static_cast<i16>(x - 40500), kTPen};
//       }
//       if (x < 43300) {
//         return (x < 41650)
//               ? ds2d{kLPen, static_cast<i16>(x - 39650)}
//               : ds2d{static_cast<i16>(x - 46150), kBPen};
//       }
//       return (x < 44950)
//             ? ds2d{static_cast<i16>(x - 40450), kTPen}
//             : ds2d{kRPen, static_cast<i16>(x - 46950)};
//     }
//     if (x < 54000) {
//       if (x < 51200) {
//         return (x < 50600)
//               ? ds2d{static_cast<i16>(x - 46100), kBPen}
//               : ds2d{static_cast<i16>(x - 55100), kTGoal};
//       }
//       return (x < 53400)
//             ? ds2d{kLGoal, static_cast<i16>(x - 52300)}
//             : ds2d{static_cast<i16>(x - 57900), kBGoal};
//     }
//     if (x < 56800) {
//       return (x < 54600)
//             ? ds2d{static_cast<i16>(x - 50100), kTGoal}
//             : ds2d{kRGoal, static_cast<i16>(x - 55700)};
//     }
//     if (x < 57400) {
//       return ds2d{static_cast<i16>(x - 52900), kBGoal};
//     }
//     // If >= 57400, resample
//     // TODO(wrsturgeon): consider, instead of resampling, sampling green
//   } while (true);
// }

// NOLINTEND(clang-diagnostic-sign-conversion)
// NOLINTEND(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

}  // namespace measure
