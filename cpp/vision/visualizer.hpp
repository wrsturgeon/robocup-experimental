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
  Tensor<H, W, 3> rgb;
  // issues with assigning to an Eigen chip, so we'll do it manually--efficiency couldn't matter less here
  for (imsize_t y = 0; y < H; ++y) {
    for (imsize_t x = 0; x < W; ++x) {
      rgb(y, x, 0) = im(y, x);
      rgb(y, x, 1) = im(y, x);
      rgb(y, x, 2) = im(y, x);
    }
  }
  for (std::size_t i = 0; i < N_samples; ++i) {
    auto Xw = measure::sample_field_lines();
    auto uv = proj(Xw);
    std::cout << "Projecting " << Xw << " to " << uv << std::endl;
    auto u = uv[0].round();
    auto v = uv[1].round();
    rgb(v, u, 0) = 255;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    rgb(v, u, 1) = 0;
    rgb(v, u, 2) = 0;
  }
  img::save<H, W, 3>(rgb.data(), "_estimate.png");
}

}  // namespace vision
