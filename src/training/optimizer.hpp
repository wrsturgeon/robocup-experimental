#pragma once

#include "util/fixed-point.hpp"

namespace training {

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
using ut32 = fp<32, 31>;
using fp32 = fp<32, 16>;
using ut16 = fp<16, 15>;
using fp16 = fp<16, 8>;

template <std::uint8_t lg_lr = 10, std::uint8_t lg_eps = 27>
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
class Adam {
 public:
  Adam();
  auto step(float grad) -> float;
};

}  // namespace training
