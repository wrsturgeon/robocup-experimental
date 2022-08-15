#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>

namespace measure {

static constexpr float kMMPerMeter = 1000.F;

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
  explicit pos_t(std::int16_t mm);
  [[nodiscard]] auto mm() const -> float { return ldexpf(internal, -lc); }
  [[nodiscard]] auto meters() const -> float { return mm() / kMMPerMeter; }
  operator std::string() const;
 private:
  static constexpr std::uint8_t lc = 1;  // lg(conversion to mm)
  // TODO(wrsturgeon): verify (prefably at compile time) that this fits within a
  std::int16_t internal;
  // SIGNED-size (std::int16_t) array
};

class Position {
 public:
  Position(std::int16_t x_mm, std::int16_t y_mm);
  operator std::string() const;
 private:
  pos_t x;
  pos_t y;
};

pos_t::pos_t(std::int16_t mm) : internal{static_cast<int16_t>(mm << lc)} {
  if constexpr (kDebug) {
    if ((internal >> lc) != mm) { throw std::overflow_error("pos_t overflow"); }
  }
}

pos_t::operator std::string() const {
  return std::to_string(static_cast<float>(internal >> lc) / kMMPerMeter) + 'm';
}

Position::Position(std::int16_t x_mm, std::int16_t y_mm) : x{x_mm}, y{y_mm} {}

Position::operator std::string() const { return '(' + x + " x, " + y + " y)"; }

auto
operator<<(std::ostream& os, Position const& p) -> std::ostream& {
  return os << p;
}

}  // namespace measure
