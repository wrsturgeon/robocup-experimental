#pragma once

#include "util/custom-int.hpp"
#include "util/fixed-point.hpp"
#include "util/units.hpp"

namespace ml {

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
template <
      std::uint8_t lg_lr = 10,
      std::uint8_t lg_ep = 27,
      std::uint8_t lg_b1 = 3,
      std::uint8_t lg_b2 = 10>
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
class Adam {
 private:
  static constexpr std::uint8_t kHalfSysBits = kSystemBits >> 1;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  using decay_t = fp<32, 32, unsigned>;
  // using decay_t = typename custom_int<kHalfSysBits, unsigned>::t;
  static constexpr decay_t full = ~decay_t{/* 0 */};
  static constexpr decay_t not_b1{full >> lg_b1}, not_b2{full >> lg_b2};
  static constexpr decay_t b1{~not_b1}, b2{~not_b2};
  static constexpr decay_t ep = full >> lg_ep;
  decay_t t{}, m{}, v{};
  decay_t decay1{b1}, decay2{b2};
 public:
  auto step(decay_t grad) -> decay_t;
};

template <std::uint8_t lg_lr, std::uint8_t lg_ep, std::uint8_t lg_b1, std::uint8_t lg_b2>
auto
Adam<lg_lr, lg_ep, lg_b1, lg_b2>::step(decay_t grad) -> decay_t {
  m = b1 * m + not_b1 * grad;
  v = b2 * v + not_b2 * (grad * grad);
  if (decay2) {
    v /= ~decay2;
    decay2 *= b2;
    if (decay1) {
      m /= ~decay1;
      decay1 *= b1;
    }
  }
  return (m / (v.sqrt() + ep)) >> lg_lr;  // theoretically faster but < precise: (m >>
                                          // lg_lr)
}

}  // namespace ml
