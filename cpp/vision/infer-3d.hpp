#pragma once

#include "vision/projection.hpp"
#include "vision/pyramid.hpp"

#include "img/types.hpp"

#include "rnd/xoshiro.hpp"

namespace vision {

[[nodiscard]] constexpr auto
projection_loss() -> img::px2d {
  return img::px2d::zero();
}

}  // namespace vision
