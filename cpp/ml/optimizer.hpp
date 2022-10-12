#pragma once

#include "fp/fixed-point.hpp"
#include "util/ints.hpp"
#include "util/units.hpp"

namespace ml {

inline constexpr u8 kDecayTBits = 32;  // TODO(wrsturgeon): evaluate 16 with a lower epsilon
using decay_t = fp::t<kDecayTBits, kDecayTBits, unsigned>;
inline constexpr u8 kLgLRDefault = 10;  // Learning rate
inline constexpr u8 kLgB1Default = 3;   // Beta 1: exponential decay rate for the first moment
inline constexpr u8 kLgB2Default = 10;  // Beta 2: exponential decay rate for the second moment
inline constexpr u8 kLgWDDefault = 7;   // Weight decay: L2 regularization multiplier
inline constexpr u8 kLgEpDefault = 27;  // Epsilon: a small constant against division by zero

template <
      typename T,
      bool republican = false,
      u8 lg_lr = kLgLRDefault,
      u8 lg_b1 = kLgB1Default,
      u8 lg_b2 = kLgB2Default,
      u8 lg_wd = kLgWDDefault,
      u8 lg_ep = kLgEpDefault>
class Adam {
 private:
  using self_t = Adam<T, republican, lg_lr, lg_b1, lg_b2, lg_wd, lg_ep>;
  static constexpr decay_t ep = decay_t::p2(-lg_ep);
  decay_t decay1 = decay_t::p2(-lg_b1);
  decay_t decay2 = decay_t::p2(-lg_b2);
  T m = T::zero();
  T v = T::zero();
  pure auto aug_m() const -> T;
 public:
  [[nodiscard]] auto step(T const& grad) -> T;
  pure auto step(T const& grad, T const& w) -> T;
};

#define ADAM_TEMPLATE template <typename T, bool republican, u8 lg_lr, u8 lg_ep, u8 lg_b1, u8 lg_b2, u8 lg_wd>

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
