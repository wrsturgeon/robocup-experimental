#pragma once

#include "measure/field-lines.hpp"
#include "vision/projection.hpp"
#include "vision/pyramid.hpp"

#include "img/io.hpp"
#include "util/uninitialized.hpp"
#include "util/units.hpp"

namespace vision {

template <ufull_t N_samples, imsize_t H = kImageH, imsize_t W = kImageW> void
display_estimate(Layer<H, W> const& im, Projection const& proj) {
  static constexpr auto vcenter = ((H + 1) >> 1);
  static constexpr auto hcenter = ((W + 1) >> 1);
  Tensor<H, W, 3> rgb;
  // issues with assigning to an Eigen chip, so we'll do it manually--efficiency couldn't matter less here
  for (imsize_t y = 0; y < H; ++y) {
    for (imsize_t x = 0; x < W; ++x) {
      rgb(y, x, Eigen::fix<0>) = im(y, x);
      rgb(y, x, Eigen::fix<1>) = im(y, x);
      rgb(y, x, Eigen::fix<2>) = im(y, x);
    }
  }
  std::array<cint<kSystemBits, signed>, 2> uv = uninitialized<std::array<cint<kSystemBits, signed>, 2>>();
  for (ufull_t i = 0; i < N_samples; ++i) {
    // do { uv = proj(measure::sample_field_lines()); } while (std::abs(uv[1]) >= vcenter or std::abs(uv[0]) >= hcenter);
    uv = proj(measure::sample_field_lines());
    if ((std::abs(uv[1]) < vcenter) and (std::abs(uv[0]) < hcenter)) {
      auto u = hcenter + uv[0];
      auto v = vcenter + uv[1];
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
      rgb(v, u, Eigen::fix<0>) = 255;
      rgb(v, u, Eigen::fix<1>) = 0;
      rgb(v, u, Eigen::fix<2>) = 0;
    }
  }
  img::save<H, W, 3>(rgb.data(), "_estimate.png");
}

}  // namespace vision
