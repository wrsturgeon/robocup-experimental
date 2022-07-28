#include "vision/pxpos.hpp"

namespace vision {



using pxidx_t = int16_t;

explicit pxpos_t(pxidx_t x_ = 0, pxidx_t y_ = 0) : x{x_}, y{y_} {}

MEMBER_INLINE operator std::string() const { return '(' +
      std::to_string(x) + "x, " +
      std::to_string(y) + "y)"; }

MEMBER_INLINE friend std::ostream& operator<<(std::ostream& os, pxpos_t const& p) { return os << static_cast<std::string>(p); }

MEMBER_INLINE uint32_t r2() const { return (
      static_cast<uint32_t>(x * x) + // overflow-safe
      static_cast<uint32_t>(y * y)); }
};



} // namespace vision
