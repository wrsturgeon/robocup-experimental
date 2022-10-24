#pragma once

#include "fp/fixed-point.hpp"
#include "trig/functions.hpp"

#include <array>

namespace vision {

inline constexpr u8 kParameterBits = 32;
using param_t = fp::t<kParameterBits, (kParameterBits >> 1), signed>;
template <std::size_t N> using param_array_t = fp::a<N, kParameterBits, (kParameterBits >> 1), signed>;

template <imsize_t u, imsize_t v> class Projection {
 private:
  param_array_t<3> t = {param_t::zero(), param_t::zero(), param_t::zero()};
  param_array_t<3> r = {param_t::zero(), param_t::zero(), param_t::zero()};
  param_array_t<3> k = {
        decltype(param_t::unit())::value(), decltype(param_t::unit())::value(), decltype(param_t::zero())::value()};
 public:
  template <u8 B, u8 I, typename S> pure auto operator()(fp::a<3, B, I, S> const& x) const noexcept -> fp::a<2, B, I, S>;
};

template <u16 u, u16 v> template <u8 B, u8 I, typename S> pure auto
Projection<u, v>::operator()(fp::a<3, B, I, S> const& X) const noexcept -> fp::a<2, B, I, S> {
  // (x y 1)^T ~ K [R t] (X Y Z 1)^T
  // (x y 1)^T ~    M    (X Y Z 1)^T
  //
  //     [ f_x  s   u  ]
  // K = [  0  f_y  v  ]
  //     [  0   0   1  ]
  //
  //     [ f_x R11 + s R21 + u R31     f_x R12 + s R22 + u R32     f_x R13 + s R23 + u R33     f_x t1 + s t2 + u t3 ]
  // M = [ f_y R21         + v R31     f_y R22         + v R32     f_y R23         + v R33     f_y t2        + v t3 ]
  //     [                     R31                         R32                         R33                       t3 ]
  //
  // x = (M11 X + M12 Y + M13 Z + M14) / (R31 X + R32 Y + R33 Z + t3)
  // y = (M21 X + M22 Y + M23 Z + M24) / (R31 X + R32 Y + R33 Z + t3)
  //

  // clang-format off
  // TODO(wrsturgeon): there's some redundancy here. worth exploiting?
  auto const R11 =   trig::cos(r[0]) * trig::cos(r[1])                                                       ;
  auto const R12 =   trig::cos(r[0]) * trig::sin(r[1]) * trig::sin(r[2]) - trig::sin(r[0]) * trig::cos(r[2]) ;
  auto const R13 =   trig::cos(r[0]) * trig::sin(r[1]) * trig::cos(r[2]) + trig::sin(r[0]) * trig::sin(r[2]) ;
  auto const R21 =   trig::sin(r[0]) * trig::cos(r[1])                                                       ;
  auto const R22 =   trig::sin(r[0]) * trig::sin(r[1]) * trig::sin(r[2]) + trig::cos(r[0]) * trig::cos(r[2]) ;
  auto const R23 =   trig::sin(r[0]) * trig::sin(r[1]) * trig::cos(r[2]) - trig::cos(r[0]) * trig::sin(r[2]) ;
  auto const R31 = +(                 -trig::sin(r[1])                                                      ); //
  auto const R32 = +(                  trig::cos(r[1]) * trig::sin(r[2])                                    ); // these three are used more than once; calculate in advance
  auto const R33 = +(                  trig::cos(r[1]) * trig::cos(r[2])                                    ); //

  auto const M11 = k[0] * R11  + k[2] * R21  + u * R31 ;
  auto const M12 = k[0] * R12  + k[2] * R22  + u * R32 ;
  auto const M13 = k[0] * R13  + k[2] * R23  + u * R33 ;
  auto const M14 = k[0] * t[0] + k[2] * t[1] + u * t[2];
  auto const M21 =               k[1] * R21  + v * R31 ;
  auto const M22 =               k[1] * R22  + v * R32 ;
  auto const M23 =               k[1] * R23  + v * R33 ;
  auto const M24 =               k[1] * t[1] + v * t[2];

  // clang-format on

  auto w = +(X[0] * R31 + X[1] * R32 + X[2] * R33 + t[2]);  // denominator
  return fp::a<2, B, I, S>{
        (+(X[0] * M11 + X[1] * M12 + X[2] * M13 + M14)) / w,
        (+(X[0] * M21 + X[1] * M22 + X[2] * M23 + M24)) / w,
  };
}

}  // namespace vision
