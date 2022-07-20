#ifndef MEASUREMENT_FIELD_LINES_HPP_
#define MEASUREMENT_FIELD_LINES_HPP_

#include <measurement/units.hpp>
#include <util/xoshiro.hpp>



static constexpr uint8_t N_FIELD_LINES = 17;

// https://spl.robocup.org/wp-content/uploads/SPL-Rules-2022.pdf
// (or, hi from the future, change 2022 unless we break the link)
static constexpr int16_t FIELD_LINES[N_FIELD_LINES][2][2] = {

  // Outermost rectangle
  {{-4500, -3000}, {-4500,  3000}}, // 6000
  {{-4500,  3000}, { 4500,  3000}}, // 9000
  {{ 4500, -3000}, { 4500,  3000}}, // 6000
  {{ 4500, -3000}, {-4500, -3000}}, // 9000

  // Center line
  {{    0, -3000}, {    0,  3000}}, // 6000 (Would love to use only 1/4 of the field but this would be duplicated)

  // Penalty area left
  {{-4500, -2000}, {-2850, -2000}}, // 1650
  {{-2850, -2000}, {-2850,  2000}}, // 4000
  {{-4500,  2000}, {-2850,  2000}}, // 1650

  // Penalty area right
  {{ 2850, -2000}, { 4500, -2000}}, // 1650
  {{ 2850, -2000}, { 2850,  2000}}, // 4000
  {{ 2850,  2000}, { 4500,  2000}}, // 1650

  // Goal area left
  {{-4500,  1100}, {-3900,  1100}}, //  600
  {{-3900, -1100}, {-3900,  1100}}, // 2200
  {{-4500, -1100}, {-3900, -1100}}, //  600

  // Goal area right
  {{ 3900,  1100}, { 4500,  1100}}, //  600
  {{ 3900, -1100}, { 3900,  1100}}, // 2200
  {{ 3900, -1100}, { 4500, -1100}}, //  600

  // Let's do the circle separately since we don't need to define it, just to sample it when needed
};

static constexpr uint16_t FIELD_PERIMETER = 57400; // Just barely under 65536 *phew*

Position sample_field_lines() {
  static constexpr uint8_t rnd_uses = BITS >> 4; // 16b each time
  static rnd::t rnd_persistent;
  static uint8_t rnd_uses_left = 0;
  do {
    if (!rnd_uses_left) {
      rnd_uses_left = rnd_uses - 1;
      rnd_persistent = rnd::next();
    } else { --rnd_uses_left; }
    uint16_t x = rnd_persistent;
    rnd_persistent >>= 16;
    // Table of values from the above lines as written
    // TODO: these could be reordered to be much more efficient
    /** indx   +=    cum
     *  [ 0]  6000   6000
     *  [ 1]  9000  15000
     *  [ 2]  6000  21000
     *  [ 3]  9000  30000
     *  [ 4]  6000  36000
     *  [ 5]  1650  37650
     *  [ 6]  4000  41650
     *  [ 7]  1650  43300
     *  [ 8]  1650  44950
     *  [ 9]  4000  48950
     *  [10]  1650  50600
     *  [11]   600  51200
     *  [12]  2200  53400
     *  [13]   600  54000
     *  [14]   600  54600
     *  [15]  2200  56800
     *  [16]   600  57400
     */
    if (x < 30000) {
      if (x < 15000) return (x < 6000)
            ? Position(-4500, x - 3000)       //     0 -  6000
            : Position(x - 10500, 3000);      //  6000 - 15000
      else return (x < 21000)
            ? Position(4500, x - 18000)       // 15000 - 21000
            : Position(x - 25500, -3000);     // 21000 - 30000
    } else if (x < 48950) {
      if (x < 37650) return (x < 36000)
            ? Position(0, x - 33000)          // 30000 - 36000
            : Position(x - 40500, -2000);     // 36000 - 37650
      else if (x < 43300) return (x < 41650)
            ? Position(-2850, x - 39650)      // 37650 - 41650
            : Position(x - 46150, 2000);      // 41650 - 43300
      else return (x < 44950)
            ? Position(x - 40450, -2000)      // 43300 - 44950
            : Position(2850, x - 46950);      // 44950 - 48950
    } else if (x < 54000) {
      if (x < 51200) return (x < 50600)
            ? Position(x - 46100, 2000)       // 48950 - 50600
            : Position(x - 55100, 1100);      // 50600 - 51200
      else return (x < 53400)
            ? Position(-3900, x - 52300)      // 51200 - 53400
            : Position(x - 57900, -1100);     // 53400 - 54000
    } else if (x < 56800) return (x < 54600)
            ? Position(x - 50100, 1100)       // 54000 - 54600
            : Position(3900, x - 55700);      // 54600 - 56800
    else if (x < 57400) return
              Position(x - 52900, -1100);     // 56800 - 57400
    // If >= 57400, resample
  } while (true);
}

#endif // MEASUREMENT_FIELD_LINES_HPP_
