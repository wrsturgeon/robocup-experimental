#pragma once

#include "measure/field-lines.hpp"
#include "vision/projection.hpp"
#include "vision/pyramid.hpp"

#include "img/io.hpp"
#include "util/units.hpp"

#include <iostream>

namespace vision {

template <std::size_t N_samples, imsize_t H = kImageH, imsize_t W = kImageW> void
display_estimate(Layer<H, W> const& im, Projection<(H >> 1), (W >> 1)> const& proj) {
  Tensor<3, W, H> rgb;
  // issues with assigning to an Eigen chip, so we'll do it manually--efficiency couldn't matter less here
  for (imsize_t y = 0; y < H; ++y) {
    for (imsize_t x = 0; x < W; ++x) {
      rgb(0, x, y) = im(y, x);
      rgb(1, x, y) = im(y, x);
      rgb(2, x, y) = im(y, x);
    }
  }
  for (std::size_t i = 0; i < N_samples; ++i) {
    auto Xw = measure::sample_field_lines();
    auto uv = proj(Xw);
    std::cout << "Projecting " << Xw << " to " << uv << std::endl;
    auto u = uv[0].round();
    auto v = uv[1].round();
    rgb(0, u, v) = 255;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    rgb(1, u, v) = 0;
    rgb(2, u, v) = 0;
  }
  img::save<H, W, 3>(rgb.data(), "_estimate.png");
}

}  // namespace vision
