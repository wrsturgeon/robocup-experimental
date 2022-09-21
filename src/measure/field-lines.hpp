#pragma once

#include "rnd/xoshiro.hpp"
#include "vision/pxpos.hpp"

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

static constexpr std::int16_t kCenter = 0;
static constexpr std::int16_t kHEdge = 4500;
static constexpr std::int16_t kVEdge = 3000;
static constexpr std::int16_t kHGoal = 3900;
static constexpr std::int16_t kVGoal = 1100;
static constexpr std::int16_t kHPen = 2850;
static constexpr std::int16_t kVPen = 2000;

static constexpr std::int16_t kLEdge = -kHEdge;
static constexpr std::int16_t kREdge = kHEdge;
static constexpr std::int16_t kTEdge = -kVEdge;
static constexpr std::int16_t kBEdge = kVEdge;
static constexpr std::int16_t kLGoal = -kHGoal;
static constexpr std::int16_t kRGoal = kHGoal;
static constexpr std::int16_t kTGoal = -kVGoal;
static constexpr std::int16_t kBGoal = kVGoal;
static constexpr std::int16_t kLPen = -kHPen;
static constexpr std::int16_t kRPen = kHPen;
static constexpr std::int16_t kTPen = -kVPen;
static constexpr std::int16_t kBPen = kVPen;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
auto
sample_field_lines() -> Position {  // NOLINT(readability-function-cognitive-complexity)
  static constexpr std::uint8_t rnd_uses = kSystemBits >> 4;
  static rnd::t rnd_state;
  static std::uint8_t rnd_uses_left;  // no need to be initialized, really
  do {
    if (!rnd_uses_left) {
      rnd_uses_left = rnd_uses - 1;
      rnd_state = rnd::next();
    } else {
      --rnd_uses_left;
    }
    auto x = static_cast<std::uint16_t>(rnd_state);
    rnd_state >>= 16;
    if (x < 30000) {
      if (x < 15000) {
        return (x < 6000)
              ? Position{kLEdge, static_cast<std::int16_t>(x - 3000)}
              : Position{static_cast<std::int16_t>(x - 10500), kBEdge};
      }
      return (x < 21000)
            ? Position{kREdge, static_cast<std::int16_t>(x - 18000)}
            : Position{static_cast<std::int16_t>(x - 25500), kTEdge};
    }
    if (x < 48950) {
      if (x < 37650) {
        return (x < 36000)
              ? Position{kCenter, static_cast<std::int16_t>(x - 33000)}
              : Position{static_cast<std::int16_t>(x - 40500), kTPen};
      }
      if (x < 43300) {
        return (x < 41650)
              ? Position{kLPen, static_cast<std::int16_t>(x - 39650)}
              : Position{static_cast<std::int16_t>(x - 46150), kBPen};
      }
      return (x < 44950)
            ? Position{static_cast<std::int16_t>(x - 40450), kTPen}
            : Position{kRPen, static_cast<std::int16_t>(x - 46950)};
    }
    if (x < 54000) {
      if (x < 51200) {
        return (x < 50600)
              ? Position{static_cast<std::int16_t>(x - 46100), kBPen}
              : Position{static_cast<std::int16_t>(x - 55100), kTGoal};
      }
      return (x < 53400)
            ? Position{kLGoal, static_cast<std::int16_t>(x - 52300)}
            : Position{static_cast<std::int16_t>(x - 57900), kBGoal};
    }
    if (x < 56800) {
      return (x < 54600)
            ? Position{static_cast<std::int16_t>(x - 50100), kTGoal}
            : Position{kRGoal, static_cast<std::int16_t>(x - 55700)};
    }
    if (x < 57400) { return Position{static_cast<std::int16_t>(x - 52900), kBGoal}; }
    // If >= 57400, resample
  } while (true);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

}  // namespace measure
