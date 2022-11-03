#pragma once

#include "measure/field-lines.hpp"
#include "vision/projection.hpp"
#include "vision/pyramid.hpp"

#include "img/io.hpp"
#include "util/units.hpp"

namespace vision {

template <ufull_t N_samples, imsize_t H = kImageH, imsize_t W = kImageW>
void
display_estimate(Pyramid<H, W> const& im, Projection const& proj) {
  static constexpr imsize_t vcenter = ((H + 1) >> 1);
  static constexpr imsize_t hcenter = ((W + 1) >> 1);
  Tensor<H, W, 3> rgb;
  // issues with assigning to an Eigen chip, so we'll do it manually--efficiency couldn't matter less here
  for (imsize_t y = 0; y < H; ++y) {
    for (imsize_t x = 0; x < W; ++x) {
      rgb(y, x, Eigen::fix<0>) = im(y, x);
      rgb(y, x, Eigen::fix<1>) = im(y, x);
      rgb(y, x, Eigen::fix<2>) = im(y, x);
    }
  }
  imsize_t y, x;
  for (ufull_t i = 0; i < N_samples; ++i) {
    auto const [u, v, w] = proj(measure::sample_field_lines());
    if (w > 0) {
      y = static_cast<imsize_t>(vcenter + v);
      x = static_cast<imsize_t>(hcenter + u);
      if ((v < H) and (u < W)) {
        rgb(y, x, Eigen::fix<0>) = 255;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        rgb(y, x, Eigen::fix<1>) = 0;
        rgb(y, x, Eigen::fix<2>) = 0;
      }
    }
  }
  img::save<H, W, 3>(rgb.data(), "_estimate.png");
}

}  // namespace vision
