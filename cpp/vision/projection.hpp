#pragma once

#if LEARN
#include "ml/optimizer.hpp"
#include "vision/pyramid.hpp"
#endif  // LEARN

#include "fp/fixed-point.hpp"
#include "measure/field-lines.hpp"
#include "trig/functions.hpp"

#include <tuple>

#ifndef NDEBUG
#include <string>
#endif

namespace vision {

inline constexpr u8 kParameterBits = 32;
inline constexpr u8 kFocalIntBits = kParameterBits >> 1;
inline constexpr u8 kRoundoffBits = 8;  // tanking precision to protect against overflow
static_assert(kFocalIntBits < (16 << 1) - 1 - kRoundoffBits);
using world_t = fp::t<kParameterBits, measure::kFieldDiagBits, signed>;
using angle_t = fp::t<kParameterBits, 0, unsigned>;
using focal_t = fp::t<kParameterBits, kFocalIntBits, unsigned>;
using vskew_t = fp::t<kParameterBits, 0, signed>;
template <ufull_t N>
using world_array_t = fp::a<N, kParameterBits, measure::kFieldDiagBits, signed>;
template <ufull_t N>
using angle_array_t = fp::a<N, kParameterBits, 0, unsigned>;
template <ufull_t N>
using focal_array_t = fp::a<N, kParameterBits, kFocalIntBits, unsigned>;

inline constexpr auto starting_pos = world_array_t<3>{
      world_t{fp::from_int(-3100)},  //
      world_t{fp::from_int(-3150)},  //
      world_t{fp::from_int(kNaoHeightMM)}};
inline constexpr auto angle_init = angle_array_t<3>{
      angle_t{0xda000000},  //
      angle_t{0x00500000},  //
      angle_t{0xB7000000}};

inline constexpr auto focal_init = focal_array_t<2>{
      focal_t{0x02200000},  //
      focal_t{0x02200000}};

using grad_t = fp::t<kSystemBits, measure::kFieldDiagBits, signed>;
// NOLINTBEGIN(misc-non-private-member-variables-in-classes,bugprone-easily-swappable-parameters,altera-struct-pack-align)
struct ProjectionGradient {
  grad_t const du_ds;
  // dv_ds = 0
  grad_t const du_df0;
  // du_df1 = 0
  // dv_df0 = 0
  grad_t const dv_df1;
  grad_t const du_dt0;
  grad_t const du_dt1;
  grad_t const du_dt2;
  grad_t const dv_dt0;
  grad_t const dv_dt1;
  grad_t const dv_dt2;
  grad_t const du_dr0;
  grad_t const du_dr1;
  grad_t const du_dr2;
  grad_t const dv_dr0;
  grad_t const dv_dr1;
  grad_t const dv_dr2;
  constexpr ProjectionGradient(
        grad_t const _du_ds,
        grad_t const _du_df0,
        grad_t const _dv_df1,
        grad_t const _du_dt0,
        grad_t const _du_dt1,
        grad_t const _du_dt2,
        grad_t const _dv_dt0,
        grad_t const _dv_dt1,
        grad_t const _dv_dt2,
        grad_t const _du_dr0,
        grad_t const _du_dr1,
        grad_t const _du_dr2,
        grad_t const _dv_dr0,
        grad_t const _dv_dr1,
        grad_t const _dv_dr2) :
        du_ds{_du_ds},
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
  world_array_t<3> t = starting_pos;     // Translation
  angle_array_t<3> r = angle_init;       // Rotation
  focal_array_t<2> f = focal_init;       // Focal length
  vskew_t /*    */ s = vskew_t::zero();  // Vertical-horizontal skew
 public:
  pure auto operator()(measure::xw_t const& X) const noexcept -> std::tuple<imsize_t, imsize_t, imsize_t>;
  pure auto grad(measure::xw_t const& X) const noexcept -> ProjectionGradient;
#if LEARN
 private:
  ml::AdamL1<world_array_t<3>> opt_t{};
  ml::AdamL1<angle_array_t<3>> opt_r{};
  ml::AdamL1<focal_array_t<2>> opt_f{};
  ml::AdamL1<vskew_t /*    */> opt_s{};
 public:
  template <imsize_t H, imsize_t W>
  INLINE void step(Pyramid<H, W> const& pyr) NOX;
#endif  // LEARN
#ifndef NDEBUG
  impure auto intrinsics() const -> std::string;
#endif
};

pure auto
Projection::operator()(measure::xw_t const& X) const noexcept -> std::tuple<imsize_t, imsize_t, imsize_t> {
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
  auto const x = static_cast<measure::mm16_t>(X[0] - t[0]);
  auto const y = static_cast<measure::mm16_t>(X[1] - t[1]);
  auto const z = static_cast<measure::mm16_t>(X[2] - t[2]);
  auto const z_c = +(x * R31 + y * R32 + z * R33);
  if (z_c <= 0) { return {0, 0, z_c}; }
  auto const z_c_l = z_c << kRoundoffBits;
  auto const x_c = +(x * R11 + y * R12 + z * R13);
  auto const y_c = +(x * R21 + y * R22 + z * R23);
  // Now to projective coordinates
  auto const f0 = +fp::t<(16 << 1), (16 << 1) - 1 - kRoundoffBits, signed>{f[0]};
  auto const f1 = +fp::t<(16 << 1), (16 << 1) - 1 - kRoundoffBits, signed>{f[1]};
  auto const sq = +fp::t<(16 << 1), (16 << 1) - 1 - kRoundoffBits, signed>{s};
  return std::tuple<imsize_t, imsize_t, imsize_t>{(x_c * f0 + y_c * sq) / z_c_l, (y_c * f1) / z_c_l, z_c};
}

pure auto
Projection::grad(measure::xw_t const& X) const noexcept -> ProjectionGradient {
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
  auto const x = static_cast<fp::t<kSystemBits - TRIG_BITS, measure::kFieldDiagBits, signed>>(X[0] - t[0]);
  auto const y = static_cast<fp::t<kSystemBits - TRIG_BITS, measure::kFieldDiagBits, signed>>(X[1] - t[1]);
  auto const z = static_cast<fp::t<kSystemBits - TRIG_BITS, measure::kFieldDiagBits, signed>>(X[2] - t[2]);
  auto const x_c = measure::mm_t{((+x) * +R11) + ((+y) * +R12) + ((+z) * +R13)};
  auto const y_c = measure::mm_t{((+x) * +R21) + ((+y) * +R22) + ((+z) * +R23)};
  auto const z_c = measure::mm16_t{measure::mm_t{((+x) * +R31) + ((+y) * +R32) + ((+z) * +R33)}};
  auto const f0 = fp::t<16, 16 - 1 - kRoundoffBits, signed>{f[0]};
  auto const f1 = fp::t<16, 16 - 1 - kRoundoffBits, signed>{f[1]};
  return ProjectionGradient{
        // /* du_ds  */ ((+y_c) << measure::kFieldDiagBits) / +z_c,
        // /* du_df0 */ ((+x_c) << measure::kFieldDiagBits) / +z_c,
        // /* dv_df1 */ ((+y_c) << measure::kFieldDiagBits) / +z_c,
        // /* du_dt0 */ (((+(x_c * R31 * f0)) << measure::kFieldDiagBits) / +(z_c.squared())) - lshift<measure::kFieldDiagBits - TRIG_BITS>((+f0) * +R11) / +z_c,
        // /* du_dt1 */ (((+(x_c * R32 * f0)) << measure::kFieldDiagBits) / +(z_c.squared())) - lshift<measure::kFieldDiagBits - TRIG_BITS>((+f0) * +R12) / +z_c,
        // /* du_dt2 */ (((+(x_c * R33 * f0)) << measure::kFieldDiagBits) / +(z_c.squared())) - lshift<measure::kFieldDiagBits - TRIG_BITS>((+f0) * +R13) / +z_c,
        // /* dv_dt0 */ (((+(y_c * R31 * f1)) << measure::kFieldDiagBits) / +(z_c.squared())) - lshift<measure::kFieldDiagBits - TRIG_BITS>((+f1) * +R21) / +z_c,
        // /* dv_dt1 */ (((+(y_c * R32 * f1)) << measure::kFieldDiagBits) / +(z_c.squared())) - lshift<measure::kFieldDiagBits - TRIG_BITS>((+f1) * +R22) / +z_c,
        // /* dv_dt2 */ (((+(y_c * R33 * f1)) << measure::kFieldDiagBits) / +(z_c.squared())) - lshift<measure::kFieldDiagBits - TRIG_BITS>((+f1) * +R23) / +z_c,
        /* du_ds  */ y_c / z_c,
        /* du_df0 */ x_c / z_c,
        /* dv_df1 */ y_c / z_c,
        /* du_dt0 */ (x_c * R31 * f0) / z_c.squared() - (f0 * R11) / z_c,
        /* du_dt1 */ (x_c * R32 * f0) / z_c.squared() - (f0 * R12) / z_c,
        /* du_dt2 */ (x_c * R33 * f0) / z_c.squared() - (f0 * R13) / z_c,
        /* dv_dt0 */ (y_c * R31 * f1) / z_c.squared() - (f1 * R21) / z_c,
        /* dv_dt1 */ (y_c * R32 * f1) / z_c.squared() - (f1 * R22) / z_c,
        /* dv_dt2 */ (y_c * R33 * f1) / z_c.squared() - (f1 * R23) / z_c,
        /* du_dr0 */ ((y_c * R11 - x_c * R12) * f0) / z_c + (x_c * (y_c * R31 - x_c * R32) * f0) / z_c.squared(),
        /* du_dr1 */ ((-x_c * cos0 * sin1 - y_c * sin0 * sin1 - z_c * cos1) * f0) / z_c - (x_c * (x_c * cos0 * cos1 * cos2 + y_c * sin0 * cos1 * cos2 - z_c * sin1 * cos2) * f0) / z_c.squared(),
        /* du_dr2 */ (x_c * y_c * f0) / z_c.squared(),
        /* dv_dr0 */ ((y_c * R21 - x_c * R22) * f1) / z_c - (y_c * (y_c * R31 - x_c * R32) * f1) / z_c.squared(),
        /* dv_dr1 */ ((x_c * cos0 * cos1 * sin2 + y_c * sin0 * cos1 * sin2 - z_c * sin1 * sin2) * f1) / z_c - (y_c * (x_c * cos0 * cos1 * cos2 + y_c * sin0 * cos1 * cos2 - z_c * sin1 * cos2) * f1) / z_c.squared(),
        /* dv_dr2 */ (y_c.squared() * f1) / z_c.squared() + f1,
  };
}

#if LEARN
template <imsize_t H, imsize_t W>
INLINE void
Projection::step(Pyramid<H, W> const& pyr) NOX {
  auto Xw = uninitialized<decltype(measure::sample_field_lines())>();
  auto Xc = uninitialized<decltype(operator()(Xw))>();
  do {
    Xw = measure::sample_field_lines();
    Xc = operator()(Xw);
  } while (std::get<2>(Xc) <= 0);
  auto const g = grad(Xw);
  auto const [du, dv] = pyr.find_imprecise(px_t{std::get<0>(Xc), std::get<1>(Xc)});
  t += opt_t(world_array_t<3>{
        g.du_dt0 * du + g.dv_dt0 * dv,  //
        g.du_dt1 * du + g.dv_dt1 * dv,  //
        g.du_dt2 * du + g.dv_dt2 * dv});
  r += opt_r(angle_array_t<3>{
        g.du_dr0 * du + g.dv_dr0 * dv,  //
        g.du_dr1 * du + g.dv_dr1 * dv,  //
        g.du_dr2 * du + g.dv_dr2 * dv});
  f += opt_f(focal_array_t<2>{
        g.du_df0 * du /* + known 0 */,  //
        /* known 0 + */ g.dv_df1 * dv});
  s += opt_s(g.du_ds * du /* + known 0 */);
}
#endif  // LEARN

#ifndef NDEBUG
impure auto
Projection::intrinsics() const -> std::string {
  std::stringstream ss;
  ss << /*      */ f[0] << ' ' << /*         */ s << ' ' << focal_t::zero() << '\n'  //
     << focal_t::zero() << ' ' << /*      */ f[1] << ' ' << focal_t::zero() << '\n'  //
     << focal_t::zero() << ' ' << focal_t::zero() << ' ' << focal_t::unit();
  return ss.str();
}
#endif

}  // namespace vision
