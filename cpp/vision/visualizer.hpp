#pragma once

#ifdef NDEBUG
#ifndef PROFILING_COMPILATION  // so we don't shoot ourselves in the foot trying to measure things
#error "Please don't use image IO in release mode <3"
#endif  // PROFILING_COMPILATION
#else   // NDEBUG

#include "measure/field-lines.hpp"
#include "vision/projection.hpp"
#include "vision/pyramid.hpp"

#include "img/io.hpp"

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
  auto uvw = uninitialized<xp_t>();
  pxidx_t u, v;
  for (ufull_t i = 0; i < N_samples; ++i) {
    uvw = proj(measure::sample_field_lines());
    if (std::get<2>(uvw) > 0) {
      u = static_cast<pxidx_t>(hcenter + std::get<0>(uvw));
      v = static_cast<pxidx_t>(vcenter + std::get<1>(uvw));
      if ((v < H) and (u < W)) {
        rgb(v, u, Eigen::fix<0>) = 255;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        rgb(v, u, Eigen::fix<1>) = 0;
        rgb(v, u, Eigen::fix<2>) = 0;
      }
    }
  }
  img::save<H, W, 3>(rgb.data(), "_estimate.png");
}

}  // namespace vision

#endif  // NDEBUG
