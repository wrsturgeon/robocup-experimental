#include "measure/units.hpp"

namespace measure {



pos_t::pos_t(int16_t mm) : internal{static_cast<int16_t>(mm << lc)} {
  assert((internal >> lc) == mm);
}

float pos_t::mm() const {
  return ldexpf(internal, -lc);
}

float pos_t::meters() const {
  return mm() / 1000.f;
}

pos_t::operator std::string() const {
  return std::to_string((internal >> lc) / 1000.f) + 'm';
}

std::ostream& operator<<(std::ostream& os, pos_t const& p) {
  return os << static_cast<std::string>(p);
}

pos_t::operator int16_t() const {
  return internal;
}



Position::Position(int16_t x_mm, int16_t y_mm) : x{x_mm}, y{y_mm} {}

Position::operator std::string() const {
  return '(' +
        static_cast<std::string>(x) + " x, " +
        static_cast<std::string>(y) + " y)";
}

std::ostream& operator<<(std::ostream& os, Position const& p) { return os << static_cast<std::string>(p); }



} // namespace measure
