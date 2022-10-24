#pragma once

#include "ml/optimizer.hpp"

#include "fp/fixed-point.hpp"
#include "img/types.hpp"

#include <array>
#include <bit>
#include <cstdint>

namespace vision {

/**
 * Simple transform to correct for lens distortion.
 * Can be manually undone & redone to verify current view against internal estimate.
 * TODO: Make these compile-time when not training, configured then saved to disk
 */
class Lens {
 private:
  static constexpr std::size_t kNWeights = 4;
  static constexpr u8 kWBits = 32;
  static constexpr u8 kWFrac = kWBits + img::px_t::f - img::px_t::i;
  using w_t = fp::t<kWBits, img::px_t::i, signed>;
  using warray_t = fp::a<kNWeights, kWBits, img::px_t::i, signed>;
  warray_t w = warray_t::zero();
  ml::Adam<warray_t, true> opt{};
 public:
  [[nodiscard]] auto undistort(img::px2d p) -> img::px2d;
  [[nodiscard]] auto redistort(img::px2d p) -> img::px2d;
};

auto
Lens::redistort(img::px2d p) -> img::px2d {
  auto r2 = p.r2();
  auto r4 = r2 * r2;
  // return (p + p[1, 0] * w[0, 1]) / (img::px_t::unit() + r2 * w[2] * r4 * w[3]);
  // The below is unfortunately faster until we've implemented expression templates:
  auto div = img::px_t::unit() + r2 * w[2] + r4 * w[3];
  return img::px2d{(p[0] + p[1] * w[0]) / div, (p[1] + p[0] * w[1]) / div};
}

}  // namespace vision
