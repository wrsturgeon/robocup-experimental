#pragma once

#include "fp/fixed-point.hpp"
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
using tparam_t = mm_exac_a<3, kParameterBits>;
using rparam_t = fp::a<3, kParameterBits, 0, signed>;
using fparam_t = fp::a<2, kParameterBits, kFocalIntBits, signed>;
using sparam_t = fp::t<kParameterBits, 0, signed>;

inline constexpr auto t_init = tparam_t{
      //
      tparam_t::scalar_t{fp::from_int(-3100)},
      tparam_t::scalar_t{fp::from_int(-3150)},
      tparam_t::scalar_t{fp::from_int(kNaoHeightMM)}};
inline constexpr auto r_init = rparam_t{
      //
      rparam_t::scalar_t{0xda000000 - 0x100000000},
      rparam_t::scalar_t{0x00500000 + 0x000000000},
      rparam_t::scalar_t{0xB7000000 - 0x100000000}};
inline constexpr auto f_init = fparam_t{
      //
      fparam_t::scalar_t{0x02200000},
      fparam_t::scalar_t{0x02200000}};
inline constexpr auto s_init = sparam_t::zero();

using compact_pos_t = fp::t<fp::kCompactBits, mm_bits, signed>;  // TODO(wrsturgeon): unfuck
// NOLINTBEGIN(misc-non-private-member-variables-in-classes,bugprone-easily-swappable-parameters,altera-struct-pack-align)
struct ProjectionGradient {
  compact_pos_t const du_dskew;
  // dskew_dv = 0
  compact_pos_t const du_df0;
  // df1_du = 0
  // df0_dv = 0
  compact_pos_t const dv_df1;
  compact_pos_t const du_dt0;
  compact_pos_t const du_dt1;
  compact_pos_t const du_dt2;
  compact_pos_t const dv_dt0;
  compact_pos_t const dv_dt1;
  compact_pos_t const dv_dt2;
  compact_pos_t const du_dr0;
  compact_pos_t const du_dr1;
  compact_pos_t const du_dr2;
  compact_pos_t const dv_dr0;
  compact_pos_t const dv_dr1;
  compact_pos_t const dv_dr2;
  constexpr ProjectionGradient(
        compact_pos_t const _du_dskew,
        compact_pos_t const _du_df0,
        compact_pos_t const _dv_df1,
        compact_pos_t const _du_dt0,
        compact_pos_t const _du_dt1,
        compact_pos_t const _du_dt2,
        compact_pos_t const _dv_dt0,
        compact_pos_t const _dv_dt1,
        compact_pos_t const _dv_dt2,
        compact_pos_t const _du_dr0,
        compact_pos_t const _du_dr1,
        compact_pos_t const _du_dr2,
        compact_pos_t const _dv_dr0,
        compact_pos_t const _dv_dr1,
        compact_pos_t const _dv_dr2) :
        du_dskew{_du_dskew},
        du_df0{_du_df0},
        dv_df1{_dv_df1},
        du_dt0{_du_dt0},
        du_dt1{_du_dt1},
        du_dt2{_du_dt2},
        dv_dt0{_dv_dt0},
        dv_dt1{_dv_dt1},
        dv_dt2{_dv_dt2},
        du_dr0{_du_dr0},
        du_dr1{_du_dr1},
        du_dr2{_du_dr2},
        dv_dr0{_dv_dr0},
        dv_dr1{_dv_dr1},
        dv_dr2{_dv_dr2} {}
};
// NOLINTEND(misc-non-private-member-variables-in-classes,bugprone-easily-swappable-parameters,altera-struct-pack-align)

class Projection {
 private:
  tparam_t t = t_init;  // Translation
  rparam_t r = r_init;  // Rotation
  fparam_t f = f_init;  // Focal length
  sparam_t s = s_init;  // Vertical-horizontal skew
 public:
  pure auto operator()(xw_t const& X) const noexcept -> xp_t;
  pure auto grad(xw_t const& X) const noexcept -> ProjectionGradient;
#ifndef NDEBUG
  impure auto intrinsics() const -> std::string;
#endif
};

pure auto
Projection::operator()(xw_t const& X) const noexcept -> xp_t {
  // INVERSE rotation (i.e. transpose since this is an orthogonal transformation)
  auto const cos0 = trig::cos(r[0]);
  auto const sin0 = trig::sin(r[0]);
  auto const cos1 = trig::cos(r[1]);
  auto const sin1 = trig::sin(r[1]);
  auto const cos2 = trig::cos(r[2]);
  auto const sin2 = trig::sin(r[2]);
  auto const R11 = cos0 * cos1;
  auto const R21 = cos0 * sin1 * sin2 - sin0 * cos2;
  auto const R31 = cos0 * sin1 * cos2 + sin0 * sin2;
  auto const R12 = sin0 * cos1;
  auto const R22 = sin0 * sin1 * sin2 + cos0 * cos2;
  auto const R32 = sin0 * sin1 * cos2 - cos0 * sin2;
  auto const R13 = -sin1;
  auto const R23 = cos1 * sin2;
  auto const R33 = cos1 * cos2;
  // No projection yet, just Euclidean transformation to camera frame
  auto const x = static_cast<compact_pos_t>(std::get<0>(X)) - t[0];
  auto const y = static_cast<compact_pos_t>(std::get<1>(X)) - t[1];
  auto const z = static_cast<compact_pos_t>(std::get<2>(X)) - t[2];
  auto const z_c = +(x * R31 + y * R32 + z * R33);
  if (z_c <= 0) { return {0, 0, z_c}; }
  auto const z_c_l = z_c << kRoundoffBits;
  auto const x_c = +(x * R11 + y * R12 + z * R13);
  auto const y_c = +(x * R21 + y * R22 + z * R23);
  // Now to projective coordinates
  auto const f0 = +fp::t<(fp::kCompactBits << 1), (fp::kCompactBits << 1) - 1 - kRoundoffBits, signed>{f[0]};
  auto const f1 = +fp::t<(fp::kCompactBits << 1), (fp::kCompactBits << 1) - 1 - kRoundoffBits, signed>{f[1]};
  auto const sq = +fp::t<(fp::kCompactBits << 1), (fp::kCompactBits << 1) - 1 - kRoundoffBits, signed>{s};
  return {(x_c * f0 + y_c * sq) / z_c_l, (y_c * f1) / z_c_l, z_c};
}

pure auto
Projection::grad(xw_t const& X) const noexcept -> ProjectionGradient {
  auto const cos0 = trig::cos(r[0]);
  auto const sin0 = trig::sin(r[0]);
  auto const cos1 = trig::cos(r[1]);
  auto const sin1 = trig::sin(r[1]);
  auto const cos2 = trig::cos(r[2]);
  auto const sin2 = trig::sin(r[2]);
  auto const R11 = cos0 * cos1;
  auto const R21 = cos0 * sin1 * sin2 - sin0 * cos2;
  auto const R31 = cos0 * sin1 * cos2 + sin0 * sin2;
  auto const R12 = sin0 * cos1;
  auto const R22 = sin0 * sin1 * sin2 + cos0 * cos2;
  auto const R32 = sin0 * sin1 * cos2 - cos0 * sin2;
  auto const R13 = -sin1;
  auto const R23 = cos1 * sin2;
  auto const R33 = cos1 * cos2;
  auto const x = static_cast<compact_pos_t>(std::get<0>(X)) - t[0];
  auto const y = static_cast<compact_pos_t>(std::get<1>(X)) - t[1];
  auto const z = static_cast<compact_pos_t>(std::get<2>(X)) - t[2];
  auto const x_c = x * R11 + y * R12 + z * R13;
  auto const y_c = x * R21 + y * R22 + z * R23;
  auto const z_c = x * R31 + y * R32 + z * R33;
  auto const f0 = fp::t<fp::kCompactBits, fp::kCompactBits - 1 - kRoundoffBits, signed>{f[0]};
  auto const f1 = fp::t<fp::kCompactBits, fp::kCompactBits - 1 - kRoundoffBits, signed>{f[1]};
  return ProjectionGradient{
        /* du_dskew */ y_c / z_c,
        /*   du_df0 */ x_c / z_c,
        /*   dv_df1 */ y_c / z_c,
        /*   du_dt0 */ (x_c * R31 * f0) / z_c.squared() - (f0 * R11) / z_c,
        /*   du_dt1 */ (x_c * R32 * f0) / z_c.squared() - (f0 * R12) / z_c,
        /*   du_dt2 */ (x_c * R33 * f0) / z_c.squared() - (f0 * R13) / z_c,
        /*   dv_dt0 */ (y_c * R31 * f1) / z_c.squared() - (f1 * R21) / z_c,
        /*   dv_dt1 */ (y_c * R32 * f1) / z_c.squared() - (f1 * R22) / z_c,
        /*   dv_dt2 */ (y_c * R33 * f1) / z_c.squared() - (f1 * R23) / z_c,
        /*   du_dr0 */ ((y * R11 - x * R12) * f0) / z_c + (x_c * (y * R31 - x * R32) * f0) / z_c.squared(),
        /*   du_dr1 */ ((-x * cos0 * sin1 - y * sin0 * sin1 - z * cos1) * f0) / z_c - (x_c * (x * cos0 * cos1 * cos2 + y * sin0 * cos1 * cos2 - z * sin1 * cos2) * f0) / z_c.squared(),
        /*   du_dr2 */ (x_c * y_c * f0) / z_c.squared(),
        /*   dv_dr0 */ ((y * R21 - x * R22) * f1) / z_c - (y_c * (y * R31 - x * R32) * f1) / z_c.squared(),
        /*   dv_dr1 */ ((x * cos0 * cos1 * sin2 + y * sin0 * cos1 * sin2 - z * sin1 * sin2) * f1) / z_c - (y_c * (x * cos0 * cos1 * cos2 + y * sin0 * cos1 * cos2 - z * sin1 * cos2) * f1) / z_c.squared(),
        /*   dv_dr2 */ (y_c.squared() * f1) / z_c.squared() + f1,
  };
}

#ifndef NDEBUG
impure auto
Projection::intrinsics() const -> std::string {
  std::stringstream ss;
  ss << /*                 */ f[0] << ' ' << /*                    */ s << ' ' << fparam_t::scalar_t::zero() << '\n'  //
     << fparam_t::scalar_t::zero() << ' ' << /*                 */ f[1] << ' ' << fparam_t::scalar_t::zero() << '\n'  //
     << fparam_t::scalar_t::zero() << ' ' << fparam_t::scalar_t::zero() << ' ' << fparam_t::scalar_t::unit();
  return ss.str();
}
#endif

}  // namespace vision
