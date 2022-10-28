#pragma once

#include "fp/fixed-point.hpp"
#include "img/types.hpp"
#include "measure/field-lines.hpp"
#include "trig/functions.hpp"

#include <array>

#ifndef NDEBUG
#include <string>
#endif

namespace vision {

inline constexpr u8 kParameterBits = 32;
inline constexpr u8 kFocalIntBits = kParameterBits >> 1;
inline constexpr u8 kRoundoffBits = 8;  // tanking precision to protect against overflow
static_assert(kFocalIntBits < (fp::kCompactBits << 1) - 1 - kRoundoffBits);
using world_t = fp::t<kParameterBits, measure::kFieldDiagBits, signed>;
using angle_t = fp::t<kParameterBits, 0, unsigned>;
using focal_t = fp::t<kParameterBits, kFocalIntBits, unsigned>;
using vskew_t = fp::t<kParameterBits, 0, signed>;
template <ufull_t N> using world_array_t = fp::a<N, kParameterBits, measure::kFieldDiagBits, signed>;
template <ufull_t N> using angle_array_t = fp::a<N, kParameterBits, 0, unsigned>;
template <ufull_t N> using focal_array_t = fp::a<N, kParameterBits, kFocalIntBits, signed>;

inline constexpr auto starting_pos = world_array_t<3>{
      // fp::from_int(-measure::kFieldWidth >> 1), fp::from_int(-measure::kFieldHeight >> 1), fp::from_int(kNaoHeightMM)};
      world_t::zero(),
      world_t::zero(),
      fp::from_int(kNaoHeightMM)};

inline constexpr auto focal_diag_init = focal_t{
      static_cast<focal_t::internal_t>((1 << (focal_t::f - 1)) * (0. + img::diag) * kNaoHeightMM / measure::kFieldDiag)};

class Projection {
 private:
  world_array_t<3> t = starting_pos;                                                         // Translation
  angle_array_t<3> r = angle_array_t<3>{angle_t::zero(), angle_t::zero(), angle_t::zero()};  // Rotation
  focal_array_t<2> f = focal_array_t<2>{focal_diag_init, focal_diag_init};                   // Focal length
  vskew_t /* */ skew = vskew_t::zero();                                                      // Vertical-horizontal skew
 public:
  template <u8 B = img::lg_diag_byte_u>
  pure auto operator()(measure::xw_t const& x) const noexcept -> std::array<cint<kSystemBits, signed>, 2>;
#ifndef NDEBUG
  impure auto intrinsics() const -> std::string;
#endif
};

template <u8 B> pure auto
Projection::operator()(measure::xw_t const& X) const noexcept -> std::array<cint<kSystemBits, signed>, 2> {
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

  // No projection yet, just Euclidean transformation to camera frame
  auto const x = static_cast<fp::t<fp::kCompactBits, measure::kFieldDiagBits, signed>>(X[0]);
  auto const y = static_cast<fp::t<fp::kCompactBits, measure::kFieldDiagBits, signed>>(X[1]);
  auto const z = static_cast<fp::t<fp::kCompactBits, measure::kFieldDiagBits, signed>>(X[2]);
  auto const x_c = +(x * R11 + y * R12 + z * R13 + t[0]);
  auto const y_c = +(x * R21 + y * R22 + z * R23 + t[1]);
  auto const z_c = +(x * R31 + y * R32 + z * R33 + t[2]) << kRoundoffBits;
  assert(z_c > 0);
  auto const f0 = +static_cast<fp::t<(fp::kCompactBits << 1), (fp::kCompactBits << 1) - 1 - kRoundoffBits, signed>>(f[0]);
  auto const f1 = +static_cast<fp::t<(fp::kCompactBits << 1), (fp::kCompactBits << 1) - 1 - kRoundoffBits, signed>>(f[1]);
  auto const sq = +static_cast<fp::t<(fp::kCompactBits << 1), (fp::kCompactBits << 1) - 1 - kRoundoffBits, signed>>(skew);
  return std::array<cint<kSystemBits, signed>, 2>{(x_c * f0 + y_c * sq) / z_c, (y_c * f1) / z_c};
}

#ifndef NDEBUG
impure auto
Projection::intrinsics() const -> std::string {
  std::stringstream ss;
  ss << /*      */ f[0] << ' ' << /*      */ skew << ' ' << focal_t::zero() << '\n'  //
     << focal_t::zero() << ' ' << /*      */ f[1] << ' ' << focal_t::zero() << '\n'  //
     << focal_t::zero() << ' ' << focal_t::zero() << ' ' << focal_t::unit();
  return ss.str();
}
#endif

}  // namespace vision
