#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>

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
  explicit pos_t(std::int16_t);
  [[nodiscard]] auto mm() const -> float;
  [[nodiscard]] auto meters() const -> float;
  explicit operator std::string() const;
  friend auto operator<<(std::ostream&, pos_t const&) -> std::ostream&;

 private:
  std::int16_t internal;
  static constexpr std::uint8_t lc = 1;  // lg(conversion to mm)
  // explicit operator std::int16_t() const;
  // TODO(wrsturgeon): verify (prefably at compile time) that this fits within a SIGNED-size (std::int16_t) array
};

class Position {
 public:
  Position(std::int16_t, std::int16_t);
  explicit operator std::string() const;
  friend auto operator<<(std::ostream&, Position const&) -> std::ostream&;

 private:
  pos_t x;
  pos_t y;
};

}  // namespace measure
