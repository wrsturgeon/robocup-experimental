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

template <typename Derived>
inline __attribute__((always_inline)) auto
max_pool(Eigen::ArrayBase<Derived> const& arr)
      -> Array<(Derived::ColsAtCompileTime >> 1), (Derived::RowsAtCompileTime >> 1)> {
  static constexpr imsize_t hw = Derived::ColsAtCompileTime >> 1;
  static constexpr imsize_t hh = Derived::RowsAtCompileTime >> 1;
  if constexpr (hw && hh) {  // TODO(wrsturgeon): randomize start +0 or +1 for odd widths
    auto tmp = arr(all, seq(0, hw, 2)).max(arr(all, seq(1, hw, 2)));
    return tmp(seq(0, hh, 2), all).max(tmp(seq(1, hh, 2), all));
  } else {
    return Array<hw, hh>{};
  }
}

template <imsize_t w, imsize_t h> class Pyramid;
template <> class Pyramid<0, 0> {
 public:
  template <typename T> explicit Pyramid(Eigen::ArrayBase<T> const& /*src*/) {}
};
template <imsize_t h> class Pyramid<0, h> {
 public:
  template <typename T> explicit Pyramid(Eigen::ArrayBase<T> const& /*src*/) {}
};
template <imsize_t w> class Pyramid<w, 0> {
 public:
  template <typename T> explicit Pyramid(Eigen::ArrayBase<T> const& /*src*/) {}
};
template <imsize_t w, imsize_t h> class Pyramid : public Array<w, h> {
 private:
  using EigenBase = Array<w, h>;
  using Map = Eigen::Map<EigenBase>;
  static constexpr imsize_t hw = w >> 1;
  static constexpr imsize_t hh = h >> 1;
  template <imsize_t w_, imsize_t h_> explicit Pyramid(Pyramid<w_, h_> const& src);
  Pyramid<hw, hh> const downsampled;  // downsample in O(1) (calculated once & saved)
 public:
  template <typename Derived> explicit Pyramid(Eigen::ArrayBase<Derived> const& src);
  auto dn() -> Pyramid<hw, hh> const& { return downsampled; }
  inline __attribute__((always_inline)) auto operator()(imsize_t x, imsize_t y) const -> dtype;
};

// TODO(wrsturgeon): Check if Gaussian blur provides noticeable benefits
// eigen-array-plugin.hpp already provides a bit-inversion operator (~a)
//   so we can flip it once and then bit-shift to the right (0.25, 0.5, 0.25)
//   which would then favor white (now flipped to 0) as it decimates bits
template <imsize_t w, imsize_t h>
template <typename Derived>
Pyramid<w, h>::Pyramid(Eigen::ArrayBase<Derived> const& src) :
      EigenBase{src},
      downsampled{max_pool(*this)} {
  static_assert(Derived::ColsAtCompileTime == w, "Pyramid width mismatch");
  static_assert(Derived::RowsAtCompileTime == h, "Pyramid height mismatch");
  // if constexpr ((Derived::ColsAtCompileTime != w) || (Derived::RowsAtCompileTime != h)) {
  //   throw std::logic_error{
  //         "Pyramid size mismatch: (" + std::to_string(w) + ", " + std::to_string(h) +
  //         ") =/= (" + std::to_string(Derived::ColsAtCompileTime) + ", " +
  //         std::to_string(Derived::RowsAtCompileTime) + ')'};
  // }
}

template <imsize_t w, imsize_t h>
inline __attribute__((always_inline)) auto
Pyramid<w, h>::operator()(imsize_t x, imsize_t y) const -> dtype {
  // TODO(wrsturgeon): if we go with center-0 indexing, use here (e.g. signed imidx_t)
  return EigenBase::operator()(y, x);
}

}  // namespace wasserstein
