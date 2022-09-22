#pragma once

#include <bit>
#include <cstdint>

namespace vision {

static constexpr std::uint8_t kLensBits = 16;
using intlens_t = custom_int<kLensBits>::signed_t;
using uintlens_t = custom_int<kLensBits>::unsigned_t;
using extlens_t = custom_int<(kLensBits << 1)>::signed_t;
using uextlens_t = custom_int<(kLensBits << 1)>::unsigned_t;
static constexpr extlens_t kOneLS = 1 << kLensBits;
static constexpr uintlens_t kDefaultInvLR = 128;

/**
 * Class holding parameters representing the idiosyncrasies of any given camera.
 * Can be manually undone & redone to verify current view against internal estimate.
 * TODO: Make these compile-time when not `TRAINING`, configured then saved to disk
 */
class Lens {
 public:
  explicit Lens(intlens_t radial, intlens_t tangential_x, intlens_t tangential_y);
  template <uextlens_t diag_sq> auto undistort(px2d p) -> px2d;
  template <uextlens_t diag_sq> auto redistort(px2d p) -> px2d;
 private:
  intlens_t rad;  // 8 bits used; extra for smooth gradient descent
  intlens_t tan_x;
  intlens_t tan_y;
  uintlens_t inv_lr{kDefaultInvLR};  // Inverse learning rate: increment over time
};

// TODO (wrsturgeon): see if we can take the below advice
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
Lens::Lens(std::int16_t radial, std::int16_t tangential_x, std::int16_t tangential_y) :
      rad{radial},
      tan_x{tangential_x},
      tan_y{tangential_y} {}

template <std::uint32_t diag_sq>
auto
Lens::redistort(px2d p) -> px2d {
  // Pack it to the brim while maintaining that the largest possible won't overflow
  static constexpr std::int8_t rs_amt = std::bit_width(diag_sq) - kLensBits;
  auto r2 = uninitialized<uintlens_t>();
  if constexpr (rs_amt < 0) {
    r2 = uintlens_t{p.r2() << -rs_amt};
  } else {
    r2 = uintlens_t{p.r2() >> rs_amt};
  }
  auto scaled = static_cast<intlens_t>(static_cast<extlens_t>(rad * r2) >> kLensBits);
  return px2d{
        static_cast<std::int16_t>(
              ((p.x << kLensBits) + (p.y * tan_y)) /
              static_cast<std::int32_t>(kOneLS + scaled)),
        static_cast<std::int16_t>(
              ((p.y << kLensBits) + (p.x * tan_x)) /
              static_cast<std::int32_t>(kOneLS + scaled))};
}

}  // namespace vision
