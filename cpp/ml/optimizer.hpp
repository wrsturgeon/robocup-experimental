#pragma once

#include "fp/fixed-point.hpp"

namespace ml {

inline constexpr u8 kDecayTBits = 16;  // TODO(wrsturgeon): evaluate 16 with a lower epsilon
using decay_t = fp::t<kDecayTBits, 0, unsigned>;
inline constexpr u8 kLgLRDefault = 5;   // Learning rate (also fractional bits in second-moment divisor)
inline constexpr u8 kLgB1Default = 3;   // Beta 1: exponential decay rate for the first moment
inline constexpr u8 kLgB2Default = 10;  // Beta 2: exponential decay rate for the second moment
inline constexpr u8 kLgWDDefault = 7;   // Weight decay: L2 regularization multiplier

template <FixedPoint T, bool republican = false, u8 lg_lr = kLgLRDefault, u8 lg_b1 = kLgB1Default, u8 lg_b2 = kLgB2Default, u8 lg_wd = kLgWDDefault>
class AdamL1 {
 private:
  using self_t = AdamL1<std::decay_t<T>, republican, lg_lr, lg_b1, lg_b2, lg_wd>;
  using rtn_t = fp::t<kSystemBits, T::i - lg_lr, typename T::signed_t>;
  decay_t decay1 = decay_t::p2<-lg_b1>();
  decay_t decay2 = decay_t::p2<-lg_b2>();
  fp::t<kSystemBits, T::i, typename T::signed_t> m{0};
  fp::t<kSystemBits, T::i, unsigned> v = fp::t<kSystemBits, T::i, unsigned>::max();
  std::decay_t<T> x_prev;
  pure auto aug_m() const -> fp::t<kSystemBits, T::i, typename T::signed_t>;
 public:
  constexpr AdamL1(std::decay_t<T> const& x_init) : x_prev{x_init} {}
  [[nodiscard]] auto step(std::decay_t<T> const& x, std::decay_t<T> const& dLdx) -> rtn_t;
  pure auto step_wt_decay(std::decay_t<T> const& x, std::decay_t<T> const& dLdx) -> rtn_t;
};

#define ADAML1_TEMPLATE template <FixedPoint T, bool republican, u8 lg_lr, u8 lg_b1, u8 lg_b2, u8 lg_wd>

ADAML1_TEMPLATE
pure auto
AdamL1<T, republican, lg_lr, lg_b1, lg_b2, lg_wd>::aug_m() const -> fp::t<kSystemBits, T::i, typename T::signed_t> {
  if constexpr (republican) { return m * decay2; }
  return m;
}

ADAML1_TEMPLATE
auto
AdamL1<T, republican, lg_lr, lg_b1, lg_b2, lg_wd>::step(std::decay_t<T> const& x, std::decay_t<T> const& dLdx) -> rtn_t {
  if constexpr (republican) {
    if (decay2 == decay_t::zero()) { return rtn_t::zero(); }
  }
  m += rshift<lg_b1>(dLdx - m);
  v += rshift<lg_b2>((x - x_prev).abs() - v);  // Actual value differences, not full-precision gradients, since data will be lost
  x_prev = x;
  // if (decay2 != decay_t::zero()) {
  //   v /= ~decay2;
  //   static_assert(FixedPoint<decltype(decay2 - (decay2 >> lg_b2))>);
  //   decay2 -= (decay2 >> lg_b2);
  //   if (decay1 != decay_t::zero()) {
  //     m /= ~decay1;
  //     decay1 -= (decay1 >> lg_b1);
  //   }
  // }
  if constexpr (republican) { decay2 -= rshift<lg_b2>(decay2); }
  using div_t = fp::t<T::b, T::b - lg_lr, unsigned>;
  static_assert(div_t::f == lg_lr);
  return rtn_t((+aug_m()) / ((+div_t{v}) | 0b1));  // 0b1 ~= epsilon
}

ADAML1_TEMPLATE
pure auto
AdamL1<T, republican, lg_lr, lg_b1, lg_b2, lg_wd>::step(std::decay_t<T> const& dLdx, std::decay_t<T> const& w) -> rtn_t {
  return step(dLdx) + (w >> lg_wd);
}

#undef ADAML1_TEMPLATE

}  // namespace ml
