#pragma once

#include "vision/pyramid.hpp"
#include "vision/projection.hpp"

#include "img/io.hpp"

namespace vision {

template <std::size_t N_samples> void display_estimate(Layer<auto, auto> const& im, Projection const& proj) {
  img::save("estimate.png", im);
}

}  // namespace vision
