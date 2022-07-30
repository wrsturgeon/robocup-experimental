#ifndef VISION_PXPOS_HPP_
#define VISION_PXPOS_HPP_

#include <iostream>
#include <stdint.h>
#include <string>

namespace vision {



using pxidx_t = int16_t;

// (0, 0) is the center of the image; expand outward from there
class pxpos_t {
public:
  pxpos_t(pxpos_t const&) = delete;
  explicit pxpos_t(pxidx_t x_ = 0, pxidx_t y_ = 0);
  inline operator std::string() const;
  inline friend std::ostream& operator<<(std::ostream& os, pxpos_t const& p);
  pxidx_t const x = 0;
  pxidx_t const y = 0;
  inline uint32_t r2() const;
};



} // namespace vision

#endif // VISION_PXPOS_HPP_
