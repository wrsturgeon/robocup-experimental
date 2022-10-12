#pragma once

#include "fp/fixed-point.hpp"
#include "util/custom-int.hpp"
#include "util/units.hpp"

namespace ml {

inline constexpr std::uint8_t kDecayTBits = 32;  // TODO(wrsturgeon): evaluate 16 with a lower epsilon
using decay_t = fp::t<kDecayTBits, kDecayTBits, unsigned>;
inline constexpr std::uint8_t kLgLRDefault = 10;  // Learning rate
inline constexpr std::uint8_t kLgB1Default = 3;   // Beta 1: exponential decay rate for the first moment
inline constexpr std::uint8_t kLgB2Default = 10;  // Beta 2: exponential decay rate for the second moment
inline constexpr std::uint8_t kLgWDDefault = 7;   // Weight decay: L2 regularization multiplier
inline constexpr std::uint8_t kLgEpDefault = 27;  // Epsilon: a small constant against division by zero

template <
      typename T,
      bool republican = false,
      std::uint8_t lg_lr = kLgLRDefault,
      std::uint8_t lg_b1 = kLgB1Default,
      std::uint8_t lg_b2 = kLgB2Default,
      std::uint8_t lg_wd = kLgWDDefault,
      std::uint8_t lg_ep = kLgEpDefault>
class Adam {
 private:
  using self_t = Adam<T, republican, lg_lr, lg_b1, lg_b2, lg_wd, lg_ep>;
  static constexpr decay_t ep = decay_t::p2(-lg_ep);
  decay_t decay1 = decay_t::p2(-lg_b1);
  decay_t decay2 = decay_t::p2(-lg_b2);
  T m = T::zero();
  T v = T::zero();
  [[nodiscard]] pure auto aug_m() const -> T;
 public:
  [[nodiscard]] auto step(T const& grad) -> T;
  [[nodiscard]] pure auto step(T const& grad, T const& w) -> T;
};

#define ADAM_TEMPLATE                                                                                                         \
  template <                                                                                                                  \
        typename T,                                                                                                           \
        bool republican,                                                                                                      \
        std::uint8_t lg_lr,                                                                                                   \
        std::uint8_t lg_ep,                                                                                                   \
        std::uint8_t lg_b1,                                                                                                   \
        std::uint8_t lg_b2,                                                                                                   \
        std::uint8_t lg_wd>

ADAM_TEMPLATE
pure auto Adam<T, republican, lg_lr, lg_ep, lg_b1, lg_b2, lg_wd>::aug_m() const -> T {
  if constexpr (republican) {
    return m * decay2;
  }
  return m;
}

ADAM_TEMPLATE
auto Adam<T, republican, lg_lr, lg_ep, lg_b1, lg_b2, lg_wd>::step(T const& grad) -> T {
  if constexpr (republican) {
    if (!decay2) {
      return T::zero();
    }
  }
  m += ((grad - m) >> lg_b1);
  v += (((grad * grad) - v) >> lg_b2);
  if (decay2) {
    v /= ~decay2;
    decay2 -= (decay2 >> lg_b2);
    if (decay1) {
      m /= ~decay1;
      decay1 -= (decay1 >> lg_b1);
    }
  }
  return (aug_m() >> lg_lr) / (v.sqrt() | 1);
}

ADAM_TEMPLATE
pure auto Adam<T, republican, lg_lr, lg_ep, lg_b1, lg_b2, lg_wd>::step(T const& grad, T const& w) -> T {
  return step(grad) + (w >> lg_wd);
}

#undef ADAM_TEMPLATE

}  // namespace ml
