#pragma once

#include "ml/optimizer.hpp"

#include "fp/fixed-point.hpp"
#include "util/units.hpp"

#include <bit>
#include <cstdint>

namespace vision {

/**
 * Class holding parameters representing the idiosyncrasies of any given camera.
 * Can be manually undone & redone to verify current view against internal estimate.
 * TODO: Make these compile-time when not training, configured then saved to disk
 */
class Lens {
 private:
  static constexpr std::size_t kNWeights = 4;
  static constexpr std::uint8_t kWBits = 32;
  static constexpr std::uint8_t kWFrac = kWBits + px_t::fbits - px_t::ibits;
  using w_t = fp::t<kWBits, kWFrac, signed>;
  using warray_t = w_t::array<kNWeights>;
  warray_t w = warray_t::zero();
  ml::Adam<warray_t, true> opt{};
 public:
  [[nodiscard]] auto undistort(px2d p) -> px2d;
  [[nodiscard]] auto redistort(px2d p) -> px2d;
};

auto Lens::redistort(px2d p) -> px2d {
  auto r2 = p.r2();
  auto r4 = r2 * r2;
  // return (p + p[1, 0] * w[0, 1]) / (px_t::unit() + r2 * w[2] * r4 * w[3]);
  // The below is unfortunately faster until we've implemented expression templates:
  auto div = px_t::unit() + r2 * w[2] + r4 * w[3];
  return px2d{(p[0] + p[1] * w[0]) / div, (p[1] + p[0] * w[1]) / div};
}

}  // namespace vision
