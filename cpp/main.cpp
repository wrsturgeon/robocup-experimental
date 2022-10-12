#include "util/ints.hpp"
#include "util/units.hpp"
#include "vision/pyramid.hpp"

#include "stb-image.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

auto main() -> int {
  // All these `assert`s will disappear when we compile the release code with -DNDEBUG
  assert((vision::Pyramid<1, 1>{Array<1, 1>::Constant(42)}(0, 0) == 42));
  assert((vision::Pyramid<1, 1>{Array<1, 1>::Constant(42)} >> 1)(0, 0) == 21);
  assert((vision::Pyramid<1, 1>{~Array<1, 1>::Constant(42)}(0, 0) == 213));
  assert((vision::Pyramid<1, 1>{~Array<1, 1>::Constant(42)} >> 1)(0, 0) == 106);
  assert((~vision::Pyramid<2, 2>{~Array<2, 2>::Zero()}.dn)(0, 0) == 0);
  assert((~vision::Pyramid<2, 2>{~Array<2, 2>::Constant(255)}.dn)(0, 0) == 255);
  int x, y, n;
  u8* im = stbi_load("../img/blurred.png", &x, &y, &n, 3);
  if (!im) {
    std::cerr << stbi_failure_reason() << std::endl;
    return 1;
  }
  assert(n == 3);
  assert(x == kImageW);
  assert(y == kImageH);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  auto d = (((ImageMap{im} >> 1) + (ImageMap{im + (kImageArea << 1)} >> 1)) >> 1) + (ImageMap{im + kImageArea} >> 1);
  std::cout << d << std::endl;
  stbi_image_free(im);
  return 0;
}
