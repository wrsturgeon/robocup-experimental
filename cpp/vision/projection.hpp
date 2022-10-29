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
      fp::from_int(-3100),  //
      fp::from_int(-3150),  //
      fp::from_int(kNaoHeightMM)};
inline constexpr auto angle_init = angle_array_t<3>{
      angle_t{0xda000000},  //
      angle_t{0x00500000},  //
      angle_t{0xB7000000}};

inline constexpr auto focal_init = focal_array_t<2>{
      focal_t{0x02200000},  //
      focal_t{0x02200000}};

class Projection {
 private:
  world_array_t<3> t = starting_pos;     // Translation
  angle_array_t<3> r = angle_init;       // Rotation
  focal_array_t<2> f = focal_init;       // Focal length
  vskew_t /* */ skew = vskew_t::zero();  // Vertical-horizontal skew
 public:
  template <u8 B = img::lg_diag_byte_u>
  pure auto operator()(measure::xw_t const& x) const noexcept -> std::array<cint<kSystemBits, signed>, 3>;
#ifndef NDEBUG
  impure auto intrinsics() const -> std::string;
#endif
};

template <u8 B> pure auto
Projection::operator()(measure::xw_t const& X) const noexcept -> std::array<cint<kSystemBits, signed>, 3> {
  // INVERSE rotation (i.e. transpose since this is an orthogonal transformation)
  auto const R11 = trig::cos(r[0]) * trig::cos(r[1]);
  auto const R21 = trig::cos(r[0]) * trig::sin(r[1]) * trig::sin(r[2]) - trig::sin(r[0]) * trig::cos(r[2]);
  auto const R31 = trig::cos(r[0]) * trig::sin(r[1]) * trig::cos(r[2]) + trig::sin(r[0]) * trig::sin(r[2]);
  auto const R12 = trig::sin(r[0]) * trig::cos(r[1]);
  auto const R22 = trig::sin(r[0]) * trig::sin(r[1]) * trig::sin(r[2]) + trig::cos(r[0]) * trig::cos(r[2]);
  auto const R32 = trig::sin(r[0]) * trig::sin(r[1]) * trig::cos(r[2]) - trig::cos(r[0]) * trig::sin(r[2]);
  auto const R13 = -trig::sin(r[1]);
  auto const R23 = trig::cos(r[1]) * trig::sin(r[2]);
  auto const R33 = trig::cos(r[1]) * trig::cos(r[2]);
  // No projection yet, just Euclidean transformation to camera frame
  auto const x = static_cast<fp::t<fp::kCompactBits, measure::kFieldDiagBits, signed>>(X[0] - t[0]);
  auto const y = static_cast<fp::t<fp::kCompactBits, measure::kFieldDiagBits, signed>>(X[1] - t[1]);
  auto const z = static_cast<fp::t<fp::kCompactBits, measure::kFieldDiagBits, signed>>(X[2] - t[2]);
  auto const z_c = +(x * R31 + y * R32 + z * R33);
  if (z_c <= 0) { return {0, 0, z_c}; }
  auto const z_c_l = z_c << kRoundoffBits;
  auto const x_c = +(x * R11 + y * R12 + z * R13);
  auto const y_c = +(x * R21 + y * R22 + z * R23);
  // Now to projective coordinates
  auto const f0 = +static_cast<fp::t<(fp::kCompactBits << 1), (fp::kCompactBits << 1) - 1 - kRoundoffBits, signed>>(f[0]);
  auto const f1 = +static_cast<fp::t<(fp::kCompactBits << 1), (fp::kCompactBits << 1) - 1 - kRoundoffBits, signed>>(f[1]);
  auto const sq = +static_cast<fp::t<(fp::kCompactBits << 1), (fp::kCompactBits << 1) - 1 - kRoundoffBits, signed>>(skew);
  return std::array<cint<kSystemBits, signed>, 3>{(x_c * f0 + y_c * sq) / z_c_l, (y_c * f1) / z_c_l, z_c};
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
