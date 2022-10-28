#pragma once

#include "fp/fixed-point.hpp"
#include "img/types.hpp"
#include "measure/field-lines.hpp"
#include "trig/functions.hpp"

#include <array>

namespace vision {

inline constexpr u8 kParameterBits = 32;
using param_t = fp::t<kParameterBits, (kParameterBits >> 1), signed>;  // the default, non-picky split of f & i
using world_t = fp::t<kParameterBits, measure::kFieldDiagBits, signed>;
using angle_t = fp::t<kParameterBits, 0, unsigned>;
template <std::size_t N> using param_array_t = fp::a<N, kParameterBits, (kParameterBits >> 1), signed>;
template <std::size_t N> using world_array_t = fp::a<N, kParameterBits, measure::kFieldDiagBits, signed>;
template <std::size_t N> using angle_array_t = fp::a<N, kParameterBits, 0, unsigned>;

static_assert(param_t::i > img::lg_diag);  // Greater-than for lens distortion

inline constexpr auto starting_pos = world_array_t<3>{
      // fp::from_int(-measure::kFieldWidth >> 1), fp::from_int(-measure::kFieldHeight >> 1), fp::from_int(kNaoHeightMM)};
      world_t::zero(),
      world_t::zero(),
      world_t::unit()};

inline constexpr auto focal_diag_init = param_t{
      // static_cast<param_t::internal_t>((1 << param_t::f) * (0. + img::diag) * kNaoHeightMM / measure::kFieldDiag)};
      static_cast<param_t::internal_t>((1 << (param_t::f - 1)) * (0. + img::diag) / measure::kFieldDiag)};

class Projection {
 private:
  world_array_t<3> t = starting_pos;                                                         // Translation
  angle_array_t<3> r = angle_array_t<3>{angle_t::zero(), angle_t::zero(), angle_t::zero()};  // Rotation
  param_array_t<3> k = param_array_t<3>{focal_diag_init, focal_diag_init, param_t::zero()};  // Intrinsics
 public:
  template <u8 B = img::lg_diag_byte_u> pure auto operator()(measure::xw_t const& x) const noexcept -> img::idxintdiff_t;
};

template <u8 B> pure auto
Projection::operator()(measure::xw_t const& X) const noexcept -> img::idxintdiff_t {
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

  // TODO(wrsturgeon): there's some redundancy here. worth exploiting?
  auto const R11 = trig::cos(r[0]) * trig::cos(r[1]);
  auto const R12 = trig::cos(r[0]) * trig::sin(r[1]) * trig::sin(r[2]) - trig::sin(r[0]) * trig::cos(r[2]);
  auto const R13 = trig::cos(r[0]) * trig::sin(r[1]) * trig::cos(r[2]) + trig::sin(r[0]) * trig::sin(r[2]);
  auto const R21 = trig::sin(r[0]) * trig::cos(r[1]);
  auto const R22 = trig::sin(r[0]) * trig::sin(r[1]) * trig::sin(r[2]) + trig::cos(r[0]) * trig::cos(r[2]);
  auto const R23 = trig::sin(r[0]) * trig::sin(r[1]) * trig::cos(r[2]) - trig::cos(r[0]) * trig::sin(r[2]);
  auto const R31 = -trig::sin(r[1]);
  auto const R32 = trig::cos(r[1]) * trig::sin(r[2]);
  auto const R33 = trig::cos(r[1]) * trig::cos(r[2]);
#ifndef NDEBUG
  std::cout << "\n    " << R11 << R12 << R13 << "\nR = " << R21 << R22 << R23 << "\n    " << R31 << R32 << R33 << std::endl;
#endif

  // No projection yet, just Euclidean transformation to camera frame
  auto const x_c = X[0] * R11 + X[1] * R12 + X[2] * R13 + t[0];
  auto const y_c = X[0] * R21 + X[1] * R22 + X[2] * R23 + t[1];
  auto const z_c = X[0] * R31 + X[1] * R32 + X[2] * R33 + t[2];
#ifndef NDEBUG
  std::cout
        << "x_c =      R11(    X[0]) +      R12(    X[1]) +      R13(    X[2]) +     t[0]\n    = " << R11 << '(' << X[0]
        << ") + " << R12 << '(' << X[1] << ") + " << R13 << '(' << X[2] << ") + " << t[0]
        << "\n    =           " << (X[0] * R11) << " +           " << (X[1] * R12) << " +           " << (X[2] * R13) << " + "
        << t[0] << "\n    = " << x_c << std::endl;
  std::cout
        << "y_c =      R21(    X[0]) +      R22(    X[1]) +      R23(    X[2]) +     t[1]\n    = " << R21 << '(' << X[0]
        << ") + " << R22 << '(' << X[1] << ") + " << R23 << '(' << X[2] << ") + " << t[1]
        << "\n    =           " << (X[0] * R21) << " +           " << (X[1] * R22) << " +           " << (X[2] * R23) << " + "
        << t[1] << "\n    = " << y_c << std::endl;
  std::cout
        << "z_c =      R31(    X[0]) +      R32(    X[1]) +      R33(    X[2]) +     t[2]\n    = " << R31 << '(' << X[0]
        << ") + " << R32 << '(' << X[1] << ") + " << R33 << '(' << X[2] << ") + " << t[2]
        << "\n    =           " << (X[0] * R31) << " +           " << (X[1] * R32) << " +           " << (X[2] * R33) << " + "
        << t[2] << "\n    = " << z_c << std::endl;
  std::cout
        << "u = (    k[0](     x_c) +     k[2](     y_c)) /      z_c\n  = (" << k[0] << '(' << x_c << ") + " << k[2] << '('
        << y_c << ")) / " << z_c << "\n  = (          " << (x_c * k[0]) << " +           " << (y_c * k[2]) << ") / " << z_c
        << "\n  =                                  " << (x_c * k[0] + y_c * k[2]) << " / " << z_c
        << "\n  = " << ((x_c * k[0]) + (y_c * k[2])) / z_c << std::endl;
  std::cout << "v =     k[1](     y_c) /      z_c\n  = " << k[1] << '(' << y_c << ") / " << z_c
            << "\n  =           " << (y_c * k[1]) << " / " << z_c << "\n  = " << (y_c * k[1]) / z_c << std::endl;
#endif
  assert(z_c > decltype(z_c)::zero());
  return img::idxintdiff_t{((x_c * k[0] + y_c * k[2]) / z_c).round(), ((y_c * k[1]) / z_c).round()};
}

}  // namespace vision
