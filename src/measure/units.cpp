#include "measure/units.hpp"

namespace measure {



pos_t(int16_t mm) : internal{static_cast<int16_t>(mm << lc)} { assert((internal >> lc) == mm); }
float mm() const { return ldexpf(internal, -lc); }
float meters() const { return mm() / 1000.f; }
operator std::string() const { return std::to_string((internal >> lc) / 1000.f) + 'm'; }
friend std::ostream& operator<<(std::ostream& os, pos_t const& p) { return os << static_cast<std::string>(p); }
explicit operator int16_t() const { return internal; }



Position(int16_t x_mm, int16_t y_mm) : x{x_mm}, y{y_mm} {}
MEMBER_INLINE operator std::string() const { return '(' +
      static_cast<std::string>(x) + " x, " +
      static_cast<std::string>(y) + " y)"; }
friend MEMBER_INLINE std::ostream& operator<<(std::ostream& os, Position const& p) { return os << static_cast<std::string>(p); }



} // namespace measure
