#ifndef MEASURE_UNITS_HPP_
#define MEASURE_UNITS_HPP_

#include <iostream>
#include <math.h>
#include <stdint.h>

#include "eigen.hpp"

namespace measure {



/**
 * Half-millimeters from the center of the field.
 * Integral type, not float, so try not to add vanishingly small amounts.
 * 1 meter := 2000 pos_t
 * Corner-to-corner field of play ~= 10.8m ~= 21,634 pos_t.
 * This type is just enough to represent negative-all-the-way-corner-to-corner,
 * which is exactly what we need to compare any two positions on the field.
 */
class pos_t {
public:
  // Purposefully no integer conversion ops: must intentionally take pos_t
  pos_t(pos_t const&) = delete;
  pos_t(int16_t mm);
  MEMBER_INLINE float mm() const;
  MEMBER_INLINE float meters() const;
  MEMBER_INLINE operator std::string() const;
  friend MEMBER_INLINE std::ostream& operator<<(std::ostream& os, pos_t const& p);
protected:
  int16_t internal;
  MEMBER_INLINE explicit operator int16_t() const;
  static constexpr uint8_t lc = 1; // lg(conversion to mm)

  // TODO: verify (and prefably autoenforce at compile time) that this fits within a SIGNED SIZE (int16_t) array

};



class Position {
public:
  Position(Position const&) = delete;
  Position(int16_t x_mm, int16_t y_mm);
  MEMBER_INLINE operator std::string() const;
  friend MEMBER_INLINE std::ostream& operator<<(std::ostream& os, Position const& p);
protected:
  pos_t x, y;
};



} // namespace measure

#endif // MEASURE_UNITS_HPP_
