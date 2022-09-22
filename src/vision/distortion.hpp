#pragma once

#include "util/custom-ints.hpp"
#include "util/units.hpp"

#include <bit>
#include <cstdint>

namespace vision {

/**
 * Class holding parameters representing the idiosyncrasies of any given camera.
 * Can be manually undone & redone to verify current view against internal estimate.
 * TODO: Make these compile-time when not `TRAINING`, configured then saved to disk
 */
class Lens {
 private:
  px_t rad;
  px_t tan_x;
  px_t tan_y;
 public:
  // NOLINTNEXTLINE(hicpp-use-equals-default,modernize-use-equals-default)
  explicit Lens() : rad{0}, tan_x{0}, tan_y{0} {}
  auto undistort(px2d p) -> px2d;
  auto redistort(px2d p) -> px2d;
};

// auto
// Lens::redistort(px2d p) -> px2d {
//   // Pack it to the brim while maintaining that the largest possible won't overflow
//   static constexpr std::int8_t rs_amt = std::bit_width(diag_sq) - kLensBits;
//   auto r2 = uninitialized<uintlens_t>();
//   if constexpr (rs_amt < 0) {  // NOLINT(bugprone-branch-clone)
//     r2 = static_cast<pxidx_t>(p.r2() << -rs_amt);
//   } else {
//     r2 = static_cast<pxidx_t>(p.r2() >> rs_amt);
//   }
//   auto scaled = static_cast<intlens_t>(static_cast<extlens_t>(rad * r2) >> kLensBits);
//   return px2d{
//         static_cast<std::int16_t>(
//               ((p.x << kLensBits) + (p.y * tan_y)) /
//               static_cast<std::int32_t>(kOneLS + scaled)),
//         static_cast<std::int16_t>(
//               ((p.y << kLensBits) + (p.x * tan_x)) /
//               static_cast<std::int32_t>(kOneLS + scaled))};
// }

}  // namespace vision
