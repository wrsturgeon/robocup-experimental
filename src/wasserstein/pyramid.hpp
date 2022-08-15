#pragma once

#include "rnd/xoshiro.hpp"

#include "eigen.hpp"

#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace wasserstein {

using dtype = std::uint8_t;
using imsize_t = std::uint16_t;  // Anything larger than this on an Atom is way too big
static_assert(std::is_unsigned<imsize_t>::value, "wasserstein::imsize_t must be unsigned");

using Eigen::seq;
using Eigen::placeholders::all;

template <imsize_t w, imsize_t h>
using Array = Eigen::Array<dtype, h, w, ((w == 1) ? Eigen::ColMajor : Eigen::RowMajor)>;

template <typename T>
inline __attribute__((always_inline)) auto
max_pool(Eigen::ArrayBase<T> const& arr)
      -> Array<(T::ColsAtCompileTime >> 1), (T::RowsAtCompileTime >> 1)> {
  static constexpr imsize_t hw = T::ColsAtCompileTime >> 1;
  static constexpr imsize_t hh = T::RowsAtCompileTime >> 1;
  static_assert(hw && hh, "wasserstein::max_pool'ing into a zero-sized Array");
  // TODO(wrsturgeon): randomize start +0 or +1 for odd widths
  auto tmp = arr(all, seq(0, hw, 2)).max(arr(all, seq(1, hw, 2)));
  return tmp(seq(0, hh, 2), all).max(tmp(seq(1, hh, 2), all));
}

template <imsize_t w, imsize_t h>  //
class Layer : public Array<w, h> {
 public:
  template <typename T> explicit Layer(Eigen::ArrayBase<T> const& src);
  inline __attribute__((always_inline)) auto operator()(imsize_t x, imsize_t y) const -> dtype;
};

template <imsize_t w, imsize_t h> class Pyramid;
template <> class Pyramid<0, 0> : public Layer<0, 0> {};
template <imsize_t h> class Pyramid<0, h> : public Layer<0, h> {};
template <imsize_t w> class Pyramid<w, 0> : public Layer<w, 0> {};
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
  Pyramid<hw, hh> const downsampled;  // downsample in O(1) (calculated once & saved)
 public:
  template <typename T> explicit Pyramid(Eigen::ArrayBase<T> const& src);
  auto dn() -> Pyramid<hw, hh> const& { return downsampled; }
};

template <imsize_t w, imsize_t h>
template <typename T>
Layer<w, h>::Layer(Eigen::ArrayBase<T> const& src) : Array<w, h>{src} {
  static_assert(T::ColsAtCompileTime == w, "Pyramid width mismatch");
  static_assert(T::RowsAtCompileTime == h, "Pyramid height mismatch");
  // if constexpr ((T::ColsAtCompileTime != w) || (T::RowsAtCompileTime != h)) {
  //   throw std::logic_error{
  //         "Pyramid size mismatch: (" + std::to_string(w) + ", " + std::to_string(h) +
  //         ") =/= (" + std::to_string(T::ColsAtCompileTime) + ", " +
  //         std::to_string(T::RowsAtCompileTime) + ')'};
  // }
}

// TODO(wrsturgeon): Check if Gaussian blur provides noticeable benefits
// eigen-array-plugin.hpp already provides a bit-inversion operator (~a)
//   so we can flip it once and then bit-shift to the right (0.25, 0.5, 0.25)
//   which would then favor white (now flipped to 0) as it decimates bits
template <imsize_t w, imsize_t h>
template <typename T>
Pyramid<w, h>::Pyramid(Eigen::ArrayBase<T> const& src) :
      Layer<w, h>{src},
      downsampled{max_pool(*this)} {}

template <imsize_t w, imsize_t h>
inline __attribute__((always_inline)) auto
Layer<w, h>::operator()(imsize_t x, imsize_t y) const -> dtype {
  // TODO(wrsturgeon): if we go with center-0 indexing, use here (e.g. signed imidx_t)
  return Array<w, h>::operator()(y, x);
}

}  // namespace wasserstein
