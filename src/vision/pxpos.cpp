#include "vision/pxpos.hpp"

namespace vision {

using pxidx_t = int16_t;

pxpos_t::pxpos_t(pxidx_t xpos, pxidx_t ypos) : x_{xpos}, y_{ypos} {}

pxpos_t::operator std::string() const {
  return '(' +
         std::to_string(x_) + "x, " +
         std::to_string(y_) + "y)";
}

auto pxpos_t::r2() const -> uint32_t {
  return (
        static_cast<uint32_t>(x_ * x_) +  // overflow-safe
        static_cast<uint32_t>(y_ * y_));
}

auto operator<<(std::ostream& os, pxpos_t const& p) -> std::ostream& {
  return os << static_cast<std::string>(p);
}

}  // namespace vision
