#pragma once

#include "vision/pxpos.hpp"

#include <cstdint>

namespace vision {

static constexpr std::uint16_t kDefaultInvLR = 128;
static constexpr std::uint8_t kLensBits =
      16;  // Don't change this without also changing ...int16_t -> ...intN_t
static constexpr std::int32_t kOneLS = 1 << kLensBits;

Lens::Lens(std::int16_t radial_, std::int16_t tangential_x_, std::int16_t tangential_y_) :
      radial{radial_},
      tangential_x{tangential_x_},
      tangential_y{tangential_y_},
      inv_lr{kDefaultInvLR} {}

template <std::uint32_t diag_sq>
auto Lens::redistort(pxpos_t px) -> pxpos_t {
  std::uint16_t r2 = rshift<lgp1(diag_sq) - kLensBits>((px.x_ * px.x_) + (px.y_ * px.y_));  // kLensBits bits, scaled to account for image size.
  auto scaled = static_cast<std::int16_t>(
        static_cast<std::int32_t>(radial * r2) >> kLensBits);
  return pxpos_t{static_cast<std::int16_t>(((px.x_ << kLensBits) + (px.y_ * tangential_y)) / static_cast<std::int32_t>(kOneLS + scaled)), static_cast<std::int16_t>(((px.y_ << kLensBits) + (px.x_ * tangential_x)) / static_cast<std::int32_t>(kOneLS + scaled))};
}

}  // namespace vision
