#ifndef IMAGE_API_HPP_
#define IMAGE_API_HPP_

#include <stdint.h>
#include <type_traits>



// Enough to store a pixel's location on some axis in any practical image.
using pxlin_t = int16_t;
using pxsqr_t = uint32_t;
static_assert(sizeof(pxsqr_t) == sizeof(pxlin_t) << 1, "pxsqr_t must be twice pxlin_t");
static_assert(!std::is_signed<pxsqr_t>::value, "pxsqr_t must be unsigned");

// {0, 0} is the center of the image; expand outward from there
class pxpos_t {
public:
  pxlin_t const x = 0;
  pxlin_t const y = 0;
  pxsqr_t r2() const { return (x * x) + (y * y); }
};



#endif // IMAGE_API_HPP_
