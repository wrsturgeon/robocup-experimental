#pragma once

#include "fp/fixed-point.hpp"
#include "measure/field-lines.hpp"
#include "trig/functions.hpp"

#ifndef NDEBUG
#include <string>
#endif

namespace vision {

inline constexpr u8 kParameterBits = 32;
inline constexpr u8 kFocalIntBits = kParameterBits >> 1;
using tparam_t = mm_exac_a<3, kParameterBits>;
using rparam_t = fp::a<3, kParameterBits, 0, signed>;
using fparam_t = fp::a<2, kParameterBits, kFocalIntBits, signed>;
using sparam_t = fp::t<kParameterBits, 0, signed>;

inline constexpr auto t_init = tparam_t{
      tparam_t::scalar_t{fp::from_int(-3100)},  //
      tparam_t::scalar_t{fp::from_int(-3150)},
      tparam_t::scalar_t{fp::from_int(kNaoHeightMM)}};
inline constexpr auto r_init = rparam_t{
      rparam_t::scalar_t{0xda000000 - 0x100000000},  //
      rparam_t::scalar_t{0x00500000 + 0x000000000},
      rparam_t::scalar_t{0xB7000000 - 0x100000000}};
inline constexpr auto f_init = fparam_t{
      fparam_t::scalar_t{0x00022000},  //
      fparam_t::scalar_t{0x00022000}};
inline constexpr auto s_init = sparam_t::zero();

using compact_pos_t = fp::t<kHalfSystemBits, mm_bits, signed>;  // TODO(wrsturgeon): unfuck
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
  using internal_t = typename compact_pos_t::internal_t;
  constexpr ProjectionGradient(internal_t _du_dskew, internal_t _du_df0, internal_t _dv_df1, internal_t _du_dt0, internal_t _du_dt1, internal_t _du_dt2, internal_t _dv_dt0, internal_t _dv_dt1, internal_t _dv_dt2, internal_t _du_dr0, internal_t _du_dr1, internal_t _du_dr2, internal_t _dv_dr0, internal_t _dv_dr1, internal_t _dv_dr2) :
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
  pure auto operator()(xw_t const& X) const NOX->xp_t;
  pure auto grad(xw_t const& X) const NOX->ProjectionGradient;
#ifndef NDEBUG
  impure auto intrinsics() const -> std::string;
#endif
};

// INVERSE rotation (i.e. transpose since this is an orthogonal transformation)
#define COMPUTE_ROTATION_MATRIX                                        \
  auto const cos0 = trig::cos(r[0]);                                   \
  auto const sin0 = trig::sin(r[0]);                                   \
  auto const cos1 = trig::cos(r[1]);                                   \
  auto const sin1 = trig::sin(r[1]);                                   \
  auto const cos2 = trig::cos(r[2]);                                   \
  auto const sin2 = trig::sin(r[2]);                                   \
  auto const R11 = trig::t{cos0 * cos1};                               \
  auto const R21 = trig::t{trig::t{cos0 * sin1} * sin2 - sin0 * cos2}; \
  auto const R31 = trig::t{trig::t{cos0 * sin1} * cos2 + sin0 * sin2}; \
  auto const R12 = trig::t{sin0 * cos1};                               \
  auto const R22 = trig::t{trig::t{sin0 * sin1} * sin2 + cos0 * cos2}; \
  auto const R32 = trig::t{trig::t{sin0 * sin1} * cos2 - cos0 * sin2}; \
  auto const R13 = trig::t{-sin1};                                     \
  auto const R23 = trig::t{cos1 * sin2};                               \
  auto const R33 = trig::t{cos1 * cos2};

// No projection yet, just Euclidean transformation to camera frame
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TRANSLATE_TO_CAMERA_COORDINATES(...)                                   \
  auto const x = compact_pos_t{std::get<0>(X)} - t[0];                         \
  auto const y = compact_pos_t{std::get<1>(X)} - t[1];                         \
  auto const z = compact_pos_t{std::get<2>(X)} - t[2];                         \
  auto const z_c = mm_exac_t<(kSystemBits >> 1)>{x * R31 + y * R32 + z * R33}; \
  auto const z_cf = z_c.floor();                                               \
  if (z_cf <= 0) { return {__VA_ARGS__}; }                                     \
  auto const x_c = mm_exac_t<(kSystemBits >> 1)>{x * R11 + y * R12 + z * R13}; \
  auto const y_c = mm_exac_t<(kSystemBits >> 1)>{x * R21 + y * R22 + z * R23}; \
  auto const f_0 = mm_exac_t<(kSystemBits >> 1)>{f[0]};                        \
  auto const f_1 = mm_exac_t<(kSystemBits >> 1)>{f[1]};

pure auto
Projection::operator()(xw_t const& X) const NOX->xp_t {
  COMPUTE_ROTATION_MATRIX
  TRANSLATE_TO_CAMERA_COORDINATES(0, 0, z_cf)
  auto const skew = mm_exac_t<(kSystemBits >> 1)>{s};
  // Now to projective coordinates (through intrinsics matrix)
  return {(x_c * f_0 + y_c * skew).internal / z_cf, (y_c * f_1).internal / z_cf, z_cf};
}

// TODO(wrsturgeon): look into returning values times z_cf so we don't have to divide 13 separate times
pure auto
Projection::grad(xw_t const& X) const NOX->ProjectionGradient {
  COMPUTE_ROTATION_MATRIX
  TRANSLATE_TO_CAMERA_COORDINATES(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
  using internal_t = typename ProjectionGradient::internal_t;
  return ProjectionGradient{
        /* du_dskew */ static_cast<internal_t>((static_cast<ifull_t>(y_c.internal) << mm_bits) / z_cf),
        /*   du_df0 */ static_cast<internal_t>((static_cast<ifull_t>(x_c.internal) << mm_bits) / z_cf),
        /*   dv_df1 */ static_cast<internal_t>((static_cast<ifull_t>(y_c.internal) << mm_bits) / z_cf),
        /*   du_dt0 */ static_cast<internal_t>(/* (x_c * R31} * f_0) / z_cf.squared() */ ((-f_0 * R11).internal << mm_bits) / z_cf),
        /*   du_dt1 */ static_cast<internal_t>(/* (x_c * R32} * f_0) / z_cf.squared() */ ((-f_0 * R12).internal << mm_bits) / z_cf),
        /*   du_dt2 */ static_cast<internal_t>(/* (x_c * R33} * f_0) / z_cf.squared() */ ((-f_0 * R13).internal << mm_bits) / z_cf),
        /*   dv_dt0 */ static_cast<internal_t>(/* (y_c * R31} * f_1) / z_cf.squared() */ ((-f_1 * R21).internal << mm_bits) / z_cf),
        /*   dv_dt1 */ static_cast<internal_t>(/* (y_c * R32} * f_1) / z_cf.squared() */ ((-f_1 * R22).internal << mm_bits) / z_cf),
        /*   dv_dt2 */ static_cast<internal_t>(/* (y_c * R33} * f_1) / z_cf.squared() */ ((-f_1 * R23).internal << mm_bits) / z_cf),
        /*   du_dr0 */ static_cast<internal_t>((compact_pos_t{y_c * R11 - x_c * R12} * f_0).internal / z_cf /* + (x_c * (y_c * R31 - x_c * R32) * f0) / z_cf.squared() */),
        /*   du_dr1 */ static_cast<internal_t>((compact_pos_t{-x_c * trig::t{cos0 * sin1} - y_c * trig::t{sin0 * sin1} - z_c * cos1} * f_0).internal / z_cf /* - (x_c * (x_c * cos0 * cos1 * cos2 + y_c * sin0 * cos1 * cos2 - z_c * sin1 * cos2) * f0) / z_cf.squared() */),
        /*   du_dr2 */ static_cast<internal_t>(/* (x_c * y_c * f0) / z_cf.squared() */ 0),
        /*   dv_dr0 */ static_cast<internal_t>((compact_pos_t{y_c * R21 - x_c * R22} * f_1).internal / z_cf /* - (y_c * (y_c * R31 - x_c * R32) * f1) / z_cf.squared() */),
        /*   dv_dr1 */ static_cast<internal_t>((compact_pos_t{x_c * trig::t{trig::t{cos0 * cos1} * sin2} + y_c * trig::t{trig::t{sin0 * cos1} * sin2} - z_c * trig::t{sin1 * sin2}} * f_1).internal / z_cf /* - (y_c * (x_c * cos0 * cos1 * cos2 + y_c * sin0 * cos1 * cos2 - z_c * sin1 * cos2) * f1) / z_cf.squared() */),
        /*   dv_dr2 */ static_cast<internal_t>(/* (y_c.squared() * f1) / z_cf.squared() + */ f_1.internal << mm_bits)};
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
