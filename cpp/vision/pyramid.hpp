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

template <typename T>
requires ((T::RowsAtCompileTime > 1) and (T::ColsAtCompileTime > 1))
pure auto pool(T const& arr) -> decltype(auto) {
  // TODO(wrsturgeon): randomize start +0 or +1 for odd widths
  using Eigen::seqN, Eigen::fix, Eigen::placeholders::all;
  static constexpr imsize_t hh = (T::RowsAtCompileTime >> 1);
  static constexpr imsize_t hw = (T::ColsAtCompileTime >> 1);
  auto tmp = arr(all, seqN(fix<0>, fix<hw>, fix<2>)).max(arr(all, seqN(fix<1>, fix<hw>, fix<2>)));
  return tmp(seqN(fix<0>, fix<hh>, fix<2>), all).max(tmp(seqN(fix<1>, fix<hh>, fix<2>), all));
}

struct bullshit {};

// TODO(wrsturgeon): listen to the pack/align tidy warning
template <imsize_t H, imsize_t W>
struct Pyramid {  // NOLINT(altera-struct-pack-align)
  using dn_t = Pyramid<(H >> 1), (W >> 1)>;
  Array<H, W> array;                                                 // NOLINT(misc-non-private-member-variables-in-classes)
  std::conditional_t<(H > 1) and (W > 1), dn_t, bullshit> const dn;  // NOLINT(misc-non-private-member-variables-in-classes)
  template <typename T>
  requires requires (T const& src) { Array<H, W>{src}; } and ((H > 1) and (W > 1) and not std::convertible_to<T, std::filesystem::path>)
  constexpr explicit Pyramid(T const& src) noexcept : array{src}, dn{pool(array)} {}
  template <typename T>
  requires requires (T const& src) { Array<H, W>{src}; } and (((H == 1) or (W == 1)) and not std::convertible_to<T, std::filesystem::path>)
  constexpr explicit Pyramid(T const& src) noexcept : array{src} {}
#ifndef NDEBUG
  explicit Pyramid(std::filesystem::path const& fpath) requires ((H == kImageH) and (W == kImageW))
        : Pyramid<H, W>{img::t<H, W, 3>{fpath}.template channel<0>()} {}
  void save(std::filesystem::path const& fpath) const;
#endif
  pure auto operator()(imsize_t i, imsize_t j) const noexcept -> decltype(auto) { return array(i, j); }
};

#ifndef NDEBUG
template <imsize_t H, imsize_t W>
void
Pyramid<H, W>::save(std::filesystem::path const& fpath) const {
  std::filesystem::create_directories(fpath);
  img::save<H, W>(array, fpath / (std::to_string(W) + 'x' + std::to_string(H) + ".png"));
  if constexpr ((H > 1) and (W > 1)) { dn.save(fpath); }
}
#endif  // NDEBUG

}  // namespace vision
