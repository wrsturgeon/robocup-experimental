#include "measure/units.hpp"

namespace measure {

static constexpr float kMMPerMeter = 1000.F;

pos_t::pos_t(int16_t mm) : internal{static_cast<int16_t>(mm << lc)} {
#if DEBUG
  if ((internal >> lc) != mm) {
    throw std::overflow_error("pos_t overflow");
  }
#endif
}

auto pos_t::mm() const -> float {
  return ldexpf(internal, -lc);
}

auto pos_t::meters() const -> float {
  return mm() / kMMPerMeter;
}

pos_t::operator std::string() const {
  return std::to_string(static_cast<float>(internal >> lc) / kMMPerMeter) + 'm';
}

auto operator<<(std::ostream& os, pos_t const& p) -> std::ostream& {
  return os << static_cast<std::string>(p);
}

// pos_t::operator int16_t() const {
//   return internal;
// }

Position::Position(int16_t x_mm, int16_t y_mm) : x{x_mm}, y{y_mm} {}

Position::operator std::string() const {
  return '(' +
         static_cast<std::string>(x) + " x, " +
         static_cast<std::string>(y) + " y)";
}

auto operator<<(std::ostream& os, Position const& p) -> std::ostream& {
  return os << static_cast<std::string>(p);
}

}  // namespace measure
