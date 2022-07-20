#ifndef MEASUREMENT_UNITS_HPP_
#define MEASUREMENT_UNITS_HPP_

#include <iostream>
#include <math.h>
#include <stdint.h>

#include <alloca.h>   // For some reason Eigen needs this, but it's probably my fault -- check later
#include <Eigen/Core> // Eigen::bfloat16

#include <util/options.hpp>



/**
 * Google Brain floating type:
 * only the first 16 bits of a 32-bit IEEE float.
 */
using bf16_t = Eigen::bfloat16;



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
  pos_t(int16_t mm) : internal(mm << lc) { assert((internal >> lc) == mm); }
  bf16_t mm() const { return bf16_t(ldexpf(internal, -lc)); }
  bf16_t meters() const { return mm() / bf16_t(1000.f); }
  operator std::string() const { return std::to_string((internal >> lc) / 1000.f) + 'm'; }
  friend std::ostream& operator<<(std::ostream& os, pos_t const& p) { return os << static_cast<std::string>(p); }
protected:
  int16_t internal;
  explicit operator int16_t() const { return internal; }
  static constexpr uint8_t lc = 1; // lg(conversion to mm)
};



class Position {
public:
  Position(Position const&) = delete;
  Position(int16_t x_mm, int16_t y_mm) : x{x_mm}, y{y_mm} {}
  operator std::string() const { return '(' +
        static_cast<std::string>(x) + "x, " +
        static_cast<std::string>(y) + "y)"; }
  friend std::ostream& operator<<(std::ostream& os, Position const& p) { return os << static_cast<std::string>(p); }
protected:
  pos_t x, y;
};



#endif // MEASUREMENT_UNITS_HPP_
