#if VISION_ENABLED
#ifndef VISION_PXPOS_HPP_
#define VISION_PXPOS_HPP_

#include <iostream>
#include <stdint.h>
#include <string>

namespace vision {



using pxidx_t = int16_t;

// {0, 0} is the center of the image; expand outward from there
class pxpos_t {
public:
  pxpos_t(pxpos_t const&) = delete;
  explicit pxpos_t(pxidx_t x_ = 0, pxidx_t y_ = 0) : x{x_}, y{y_} {}
  MEMBER_INLINE operator std::string() const { return '(' +
        std::to_string(x) + "x, " +
        std::to_string(y) + "y)"; }
  MEMBER_INLINE friend std::ostream& operator<<(std::ostream& os, pxpos_t const& p) { return os << static_cast<std::string>(p); }
  pxidx_t const x = 0;
  pxidx_t const y = 0;
  MEMBER_INLINE uint32_t r2() const { return (
        static_cast<uint32_t>(x * x) + // overflow-safe
        static_cast<uint32_t>(y * y)); }
};



} // namespace vision

#endif // VISION_PXPOS_HPP_

#else // VISION_ENABLED
#pragma message("Skipping pxpos.hpp; vision module disabled")
#endif // VISION_ENABLED
