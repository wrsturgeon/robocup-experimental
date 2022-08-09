#include "vision/pxpos.hpp"

namespace vision {

using pxidx_t = int16_t;

pxpos_t::pxpos_t(pxidx_t x_, pxidx_t y_) : x{x_}, y{y_} {}

pxpos_t::operator std::string() const {
  return '(' +
         std::to_string(x) + "x, " +
         std::to_string(y) + "y)";
}

auto pxpos_t::r2() const -> uint32_t {
  return (
        static_cast<uint32_t>(x * x) +  // overflow-safe
        static_cast<uint32_t>(y * y));
}

auto operator<<(std::ostream& os, pxpos_t const& p) -> std::ostream& {
  return os << static_cast<std::string>(p);
}

}  // namespace vision
