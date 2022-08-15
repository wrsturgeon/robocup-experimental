#pragma once

#include "rnd/xoshiro.hpp"

#include "eigen.hpp"

#include <stdexcept>
#include <type_traits>

namespace vision {

template <typename T>
inline __attribute__((always_inline)) auto
max_pool(Eigen::ArrayBase<T> const& arr)
      -> Array<(T::ColsAtCompileTime >> 1), (T::RowsAtCompileTime >> 1)> {
  static constexpr imsize_t hw = T::ColsAtCompileTime >> 1;
  static constexpr imsize_t hh = T::RowsAtCompileTime >> 1;
  static_assert(hw && hh, "vision::max_pool'ing into a zero-sized Array");
  // TODO(wrsturgeon): randomize start +0 or +1 for odd widths
  using Eigen::seq;
  using Eigen::placeholders::all;
  auto tmp = arr(all, seq(0, hw, 2)).max(arr(all, seq(1, hw, 2)));
  return tmp(seq(0, hh, 2), all).max(tmp(seq(1, hh, 2), all));
}

template <imsize_t w, imsize_t h>  //
class Layer : public Array<w, h> {
 public:
  template <typename T> explicit Layer(Eigen::ArrayBase<T> const& src);
  inline __attribute__((always_inline)) auto operator()(pxidx_t x, pxidx_t y) const -> dtype;
};

template <imsize_t w, imsize_t h> class Pyramid;
template <> class Pyramid<1, 1> : public Layer<1, 1> {
 public:
  template <typename T> explicit Pyramid(Eigen::ArrayBase<T> const& src) : Layer<1, 1>{src} {}
};
template <imsize_t h> class Pyramid<1, h> : public Layer<1, h> {
 public:
  template <typename T> explicit Pyramid(Eigen::ArrayBase<T> const& src) : Layer<1, h>{src} {}
};
template <imsize_t w> class Pyramid<w, 1> : public Layer<w, 1> {
 public:
  template <typename T> explicit Pyramid(Eigen::ArrayBase<T> const& src) : Layer<w, 1>{src} {}
};
template <imsize_t w, imsize_t h> class Pyramid : public Layer<w, h> {
 private:
  static constexpr imsize_t hw = w >> 1;
  static constexpr imsize_t hh = h >> 1;
 public:
  template <typename T> explicit Pyramid(Eigen::ArrayBase<T> const& src);
  Pyramid<hw, hh> const dn;  // downsample in O(1) (calculated once & saved)
};

template <imsize_t w, imsize_t h>
template <typename T>
Layer<w, h>::Layer(Eigen::ArrayBase<T> const& src) : Array<w, h>{src} {
  static_assert(w && h, "Instantiating vision::Layer of zero size");
  static_assert(T::ColsAtCompileTime == w, "Pyramid width mismatch");
  static_assert(T::RowsAtCompileTime == h, "Pyramid height mismatch");
}

// TODO(wrsturgeon): Check if Gaussian blur provides noticeable benefits
// Already made an easy bit-inversion operator in eigen-array-plugin.hpp
//   so we can flip it once and then bit-shift to the right (0.25, 0.5, 0.25)
//   which would then favor white (now flipped to 0) as it decimates bits
template <imsize_t w, imsize_t h>
template <typename T>
Pyramid<w, h>::Pyramid(Eigen::ArrayBase<T> const& src) :
      Layer<w, h>{src},
      dn{max_pool(*this)} {}

template <imsize_t w, imsize_t h>
inline __attribute__((always_inline)) auto
Layer<w, h>::operator()(pxidx_t x, pxidx_t y) const -> dtype {
  // TODO(wrsturgeon): if we go with center-0 indexing, adjust here
  return Array<w, h>::operator()(y, x);
}

}  // namespace vision
