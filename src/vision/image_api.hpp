#ifndef IMAGE_API_HPP_
#define IMAGE_API_HPP_

#include <stdint.h>



// Enough to store a pixel's location on some axis in any practical image.
using pxlin_t = int16_t;

// {0, 0} is the center of the image; expand outward from there
struct pxpos_t {
  pxlin_t x = 0;
  pxlin_t y = 0;
};



#endif // IMAGE_API_HPP_
