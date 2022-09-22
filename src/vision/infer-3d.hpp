#pragma once

#include "vision/distortion.hpp"
#include "vision/pyramid.hpp"

#include "rnd/xoshiro.hpp"

#include "util/units.hpp"

namespace vision {

auto
projection_loss() -> px2d {
  return px2d{0, 0};
}

}  // namespace vision
