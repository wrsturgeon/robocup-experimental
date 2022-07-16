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
 * On a standard SPL field, 1 metre := 2048 dist_t.
 * Explanation for those interested:
 * - Field dims = {10.4m, 7.4m}; corner-to-corner < 12.8m.
 * - Range of int16_t = [-32768, 32767].
 * - 32767 / 12.8 ~= 2560 < 2048 = (1 << 11).
 *   - Bit shifting is easy and fast
 */
class dist_t {
public:
  bf16_t meters() const { return static_cast<bf16_t>(ldexpf(internal, this->lg_cnv)); }
  operator std::string() const { return std::to_string(this->meters()) + 'm'; }
protected:
  int16_t internal = 0;
  explicit operator int16_t() const { return internal; }
  static constexpr int8_t lg_cnv = -11;
};



#endif // MEASUREMENT_UNITS_HPP_