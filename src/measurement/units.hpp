#ifndef MEASUREMENT_UNITS_HPP_
#define MEASUREMENT_UNITS_HPP_

#include <stdint.h>
#include <math.h>

#include <Eigen/Core> // Eigen::bfloat16



/**
 * Google Brain floating type:
 * only the first 16 bits of a 32-bit IEEE float.
 */
using bf16_t = Eigen::bfloat16;



/**
 * Distance from the center of the field.
 * Integral type, not float, so try not to add vanishingly small amounts.
 * On a standard SPL field, 1 metre := 2048 idx_t.
 * Explanation for those interested:
 * - Field dims = {10.4m, 7.4m}; corner-to-corner < 12.8m.
 * - Range of int16_t = [-32768, 32767].
 * - 32767 / 12.8 ~= 2560 < 2048 = (1 << 11).
 *   - Bit shifting is easy and fast
 */
class idx_t {
public:
  idx_t(idx_t const&) = delete;
  idx_t() : internal{0} {}
  bf16_t meters() const { return static_cast<bf16_t>(ldexpf(internal, lg_cnv)); }
  operator std::string() const { return std::to_string(meters()) + 'm'; }
protected:
  int16_t internal = 0;
  explicit operator int16_t() const { return internal; }
  static constexpr int8_t lg_cnv = -11;
};



class pos_t {
public:
  pos_t(pos_t const&) = delete;
  operator std::string() const { return '(' +
        static_cast<std::string>(x) + "x, " +
        static_cast<std::string>(y) + "y, " +
        static_cast<std::string>(z) + "z)"; }
protected:
  idx_t x, y, z;
};



#endif // MEASUREMENT_UNITS_HPP_
