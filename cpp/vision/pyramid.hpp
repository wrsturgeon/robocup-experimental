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

template <EigenExpressible T> requires ((T::RowsAtCompileTime > 1) and (T::ColsAtCompileTime > 1))
pure auto min_pool(T const& arr) -> decltype(auto) {
  // TODO(wrsturgeon): randomize start +0 or +1 for odd widths
  using Eigen::seqN;
  using Eigen::placeholders::all;
  static constexpr img::imsize_t hh = (T::RowsAtCompileTime >> 1);
  static constexpr img::imsize_t hw = (T::ColsAtCompileTime >> 1);
  auto tmp = arr(all, seqN(0, hw, 2)).min(arr(all, seqN(1, hw, 2)));
  // TODO(wrsturgeon): why isn't this actually taking the minimum???
  return tmp(seqN(0, hh, 2), all).min(tmp(seqN(1, hh, 2), all));
}

#define LAYER_BASE Array<h, w>
template <img::imsize_t h, img::imsize_t w>  //
class Layer : public LAYER_BASE {
 public:
  using Derived = LAYER_BASE;
#undef LAYER_BASE
  using self_t = Layer<h, w>;
  template <ArrayExpressible<h, w> T> constexpr explicit Layer(T const& src) noexcept : Derived{src} {}
#ifndef NDEBUG
  explicit Layer(std::filesystem::path const& fpath) requires ((h == kImageH) and (w == kImageW));
#endif
};

struct bullshit {};

template <img::imsize_t h, img::imsize_t w> class Pyramid : public Layer<h, w> {
 private:
  static constexpr img::imsize_t hh = (h >> 1);
  static constexpr img::imsize_t hw = (w >> 1);
 public:
  // TODO(wrsturgeon): Check if Gaussian blur provides noticeable benefits
  using typename Layer<h, w>::Derived;
  template <EigenExpressible T> requires ((hh > 0) and (hw > 0))
  constexpr explicit Pyramid(T const& src) noexcept : Layer<h, w>{src}, dn{min_pool(*this)} {}
  template <EigenExpressible T> constexpr explicit Pyramid(T const& src) noexcept : Layer<h, w>{src} {}
#ifndef NDEBUG
  explicit Pyramid(std::filesystem::path const& fpath) requires ((h == kImageH) and (w == kImageW));
  template <bool first = true> void save(std::filesystem::path const& fpath) const;
#endif
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  std::conditional_t<hh and hw, Pyramid<hh, hw>, bullshit> const dn;  // downsample in O(1) (calculated once & saved)
};

#ifndef NDEBUG
template <img::imsize_t h, img::imsize_t w>
Layer<h, w>::Layer(std::filesystem::path const& fpath) requires ((h == kImageH) and (w == kImageW))
      : self_t{img::t<h, w, 3>{fpath}.collapse()} {
  // For some reason the data in the self_t constructor is getting swapped to column-major
  // So the image looks like it was meant to have been written to WxH but it got written to HxW
  // TODO(wrsturgeon): so theoretically if we interpret it as column-major it would be correct
  img::save<h, w>(*this, std::filesystem::current_path() / ("LAYER_" + std::to_string(w) + 'x' + std::to_string(h) + ".png"));
}
template <img::imsize_t h, img::imsize_t w>
Pyramid<h, w>::Pyramid(std::filesystem::path const& fpath) requires ((h == kImageH) and (w == kImageW))
      : Layer<h, w>{fpath}, dn{min_pool(*this)} {}
template <img::imsize_t h, img::imsize_t w> template <bool first> void
Pyramid<h, w>::save(std::filesystem::path const& fpath) const {
  if constexpr (first) { std::filesystem::create_directories(fpath); }
  img::save<h, w>(*this, fpath / (std::to_string(w) + 'x' + std::to_string(h) + ".png"));
  if constexpr (hw and hh) { dn.template save<false>(fpath); }
}
#endif  // NDEBUG

}  // namespace vision
