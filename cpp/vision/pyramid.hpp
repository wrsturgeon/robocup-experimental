#pragma once

#ifndef NDEBUG
#include "img/io.hpp"
#endif

#include "rnd/xoshiro.hpp"
#include "util/units.hpp"

#include "eigen.hpp"

#include <stdexcept>
#include <type_traits>

// TODO(wrsturgeon): implement pyramid constructor from image filepath, collapse to one channel and bit-invert

namespace vision {

using dtype = u8;

template <EigenExpression T> requires ((T::RowsAtCompileTime > 1) and (T::ColsAtCompileTime > 1))
pure auto max_pool(T const& arr) -> decltype(auto) {
  // TODO(wrsturgeon): randomize start +0 or +1 for odd widths
  using Eigen::seqN;
  using Eigen::placeholders::all;
  static constexpr imsize_t hw = (T::ColsAtCompileTime >> 1);
  static constexpr imsize_t hh = (T::RowsAtCompileTime >> 1);
  auto tmp = arr(all, seqN(1, hw, 2)).max(arr(all, seqN(1, hw, 2)));
  return tmp(seqN(1, hh, 2), all).max(tmp(seqN(1, hh, 2), all));
}

template <imsize_t w, imsize_t h>  //
class Layer : public Array<w, h> {
 public:
  using Derived = Array<w, h>;
  template <ArrayExpression<w, h> T> constexpr explicit Layer(T const& src) noexcept : Derived{src} {}
#ifndef NDEBUG
  constexpr explicit Layer(char const* fpath) requires (w == kImageW && h == kImageH);
#endif
  // pure auto operator()(pxint_t x, pxint_t y) const -> decltype(auto);
  // TODO(wrsturgeon): consider using (fractional) px_t / px2d above
};

struct bullshit {};

template <imsize_t w, imsize_t h> class Pyramid : public Layer<w, h> {
 private:
  static constexpr imsize_t hw = (w >> 1);
  static constexpr imsize_t hh = (h >> 1);
 public:
  // TODO(wrsturgeon): Check if Gaussian blur provides noticeable benefits
  using typename Layer<w, h>::Derived;
  template <EigenExpression T> requires ((hw > 0) and (hh > 0))
  constexpr explicit Pyramid(T const& src) noexcept : Layer<w, h>{src}, dn{max_pool(*this)} {}
  template <EigenExpression T> constexpr explicit Pyramid(T const& src) noexcept : Layer<w, h>{src} {}
#ifndef NDEBUG
  constexpr explicit Pyramid(char const* fpath) requires (w == kImageW and h == kImageH);
#endif
  // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
  std::conditional_t<hw and hh, Pyramid<hw, hh>, bullshit> const dn;  // downsample in O(1) (calculated once & saved)
};

#ifndef NDEBUG
template <imsize_t w, imsize_t h>
constexpr Layer<w, h>::Layer(char const* const fpath) requires (w == kImageW and h == kImageH)
      : Array<w, h>{img::t<3>{fpath}.collapse()} {}
template <imsize_t w, imsize_t h>
constexpr Pyramid<w, h>::Pyramid(char const* const fpath) requires (w == kImageW and h == kImageH)
      : Layer<w, h>{fpath}, dn{max_pool(*this)} {}
#endif  // NDEBUG

// template <imsize_t w, imsize_t h> constexpr auto
// Layer<w, h>::operator()(pxint_t x, pxint_t y) const -> decltype(auto) {
//   // TODO(wrsturgeon): if we go with center-0 indexing, adjust here
//   return Array<w, h>::operator()(y, x);
// }

}  // namespace vision
