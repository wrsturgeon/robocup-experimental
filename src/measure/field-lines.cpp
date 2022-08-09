#include "measure/field-lines.hpp"

namespace measure {

auto sample_field_lines() -> Position {
  static constexpr uint8_t rnd_uses = BITS >> 4; // 16b each time
  static rnd::t rnd_state;
  static uint8_t rnd_uses_left; // Not initializing: compiler doesn't need the extra first-time-calling-or-not flag
  do {
    if (!rnd_uses_left) {
      rnd_uses_left = rnd_uses - 1;
      rnd_state = rnd::next();
    } else {
      --rnd_uses_left;
    }
    uint16_t x = static_cast<uint16_t>(rnd_state);
    rnd_state >>= 16;
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
    if (x < 30000) {
      if (x < 15000)
        return (x < 6000)
                     ? Position{static_cast<int16_t>(-4500), static_cast<int16_t>(x - 3000)}  // [ 1] :     0 -  6000
                     : Position{static_cast<int16_t>(x - 10500), static_cast<int16_t>(3000)}; // [ 2] :  6000 - 15000
      else
        return (x < 21000)
                     ? Position{static_cast<int16_t>(4500), static_cast<int16_t>(x - 18000)}   // [ 3] : 15000 - 21000
                     : Position{static_cast<int16_t>(x - 25500), static_cast<int16_t>(-3000)}; // [ 4] : 21000 - 30000
    } else if (x < 48950) {
      if (x < 37650)
        return (x < 36000)
                     ? Position{static_cast<int16_t>(0), static_cast<int16_t>(x - 33000)}      // [ 5] : 30000 - 36000
                     : Position{static_cast<int16_t>(x - 40500), static_cast<int16_t>(-2000)}; // [ 6] : 36000 - 37650
      else if (x < 43300)
        return (x < 41650)
                     ? Position{static_cast<int16_t>(-2850), static_cast<int16_t>(x - 39650)} // [ 7] : 37650 - 41650
                     : Position{static_cast<int16_t>(x - 46150), static_cast<int16_t>(2000)}; // [ 8] : 41650 - 43300
      else
        return (x < 44950)
                     ? Position{static_cast<int16_t>(x - 40450), static_cast<int16_t>(-2000)} // [ 9] : 43300 - 44950
                     : Position{static_cast<int16_t>(2850), static_cast<int16_t>(x - 46950)}; // [10] : 44950 - 48950
    } else if (x < 54000) {
      if (x < 51200)
        return (x < 50600)
                     ? Position{static_cast<int16_t>(x - 46100), static_cast<int16_t>(2000)}   // [11] : 48950 - 50600
                     : Position{static_cast<int16_t>(x - 55100), static_cast<int16_t>(-1100)}; // [12] : 50600 - 51200
      else
        return (x < 53400)
                     ? Position{static_cast<int16_t>(-3900), static_cast<int16_t>(x - 52300)} // [13] : 51200 - 53400
                     : Position{static_cast<int16_t>(x - 57900), static_cast<int16_t>(1100)}; // [14] : 53400 - 54000
    } else if (x < 56800)
      return (x < 54600)
                   ? Position{static_cast<int16_t>(x - 50100), static_cast<int16_t>(-1100)} // [15] : 54000 - 54600
                   : Position{static_cast<int16_t>(3900), static_cast<int16_t>(x - 55700)}; // [16] : 54600 - 56800
    else if (x < 57400)
      return Position{static_cast<int16_t>(x - 52900), static_cast<int16_t>(1100)}; // [17] : 56800 - 57400
    // If >= 57400, resample
  } while (true);
}

} // namespace measure
