#pragma once

#ifndef NDEBUG
#include "img/io.hpp"
#endif

#include "img/types.hpp"
#include "rnd/xoshiro.hpp"
#include "util/units.hpp"

#include "eigen.hpp"

#include <filesystem>
#include <stdexcept>
#include <type_traits>

namespace vision {

using dtype = u8;

template <EigenExpressible T>
requires ((T::RowsAtCompileTime > 1) and (T::ColsAtCompileTime > 1))
pure auto pool(T const& arr) -> decltype(auto) {
  // TODO(wrsturgeon): randomize start +0 or +1 for odd widths
  using Eigen::seqN, Eigen::fix, Eigen::placeholders::all;
  static constexpr imsize_t hh = (T::RowsAtCompileTime >> 1);
  static constexpr imsize_t hw = (T::ColsAtCompileTime >> 1);
  auto tmp = arr(all, seqN(fix<0>, fix<hw>, fix<2>)).max(arr(all, seqN(fix<1>, fix<hw>, fix<2>)));
  return tmp(seqN(fix<0>, fix<hh>, fix<2>), all).max(tmp(seqN(fix<1>, fix<hh>, fix<2>), all));
}

#define LAYER_BASE Array<H, W>
template <imsize_t H = kImageH, imsize_t W = kImageW>
class Layer : public LAYER_BASE {
 public:
  using Derived = LAYER_BASE;
#undef LAYER_BASE
  using self_t = Layer<H, W>;
  template <ArrayExpressible<H, W> T>
  constexpr explicit Layer(T const& src) noexcept : Derived{src} {}
#ifndef NDEBUG
  explicit Layer(std::filesystem::path const& fpath) requires ((H == kImageH) and (W == kImageW));
#endif
};

struct bullshit {};

// TODO(wrsturgeon): listen to the pack/align tidy warning
template <imsize_t H, imsize_t W>
struct Pyramid : public Layer<H, W> {  // NOLINT(altera-struct-pack-align)
  using Derived = typename Layer<H, W>::Derived;
  template <EigenExpressible T>
  requires ((H > 1) and (W > 1))
  constexpr explicit Pyramid(T const& src) noexcept : Layer<H, W>{src}, dn{pool(*this)} {}
  template <EigenExpressible T>
  constexpr explicit Pyramid(T const& src) noexcept : Layer<H, W>{src} {}
#ifndef NDEBUG
  explicit Pyramid(std::filesystem::path const& fpath) requires ((H == kImageH) and (W == kImageW));
  void save(std::filesystem::path const& fpath) const;
#endif
  using dn_t = Pyramid<(H >> 1), (W >> 1)>;
  std::conditional_t<(H > 1) and (W > 1), dn_t, bullshit> const dn;  // NOLINT(misc-non-private-member-variables-in-classes)
};

#ifndef NDEBUG
template <imsize_t H, imsize_t W>
Layer<H, W>::Layer(std::filesystem::path const& fpath) requires ((H == kImageH) and (W == kImageW))
      : self_t{img::t<H, W, 3>{fpath}.template channel<0>()} {
  // For some reason the data in the self_t constructor is getting swapped to column-major
  // So the image looks like it was meant to have been written to WxH but it got written to HxW
  // TODO(wrsturgeon): so theoretically if we interpret it as column-major it would be correct
  img::save<H, W>(*this, std::filesystem::current_path() / ("LAYER_" + std::to_string(W) + 'x' + std::to_string(H) + ".png"));
}
template <imsize_t H, imsize_t W>
Pyramid<H, W>::Pyramid(std::filesystem::path const& fpath) requires ((H == kImageH) and (W == kImageW))
      : Layer<H, W>{fpath}, dn{pool(*this)} {}
template <imsize_t H, imsize_t W>
void
Pyramid<H, W>::save(std::filesystem::path const& fpath) const {
  std::filesystem::create_directories(fpath);
  img::save<H, W>(*this, fpath / (std::to_string(W) + 'x' + std::to_string(H) + ".png"));
  if constexpr ((H > 1) and (W > 1)) { dn.save(fpath); }
}
#endif  // NDEBUG

}  // namespace vision
