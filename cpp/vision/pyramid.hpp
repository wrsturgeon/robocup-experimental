#pragma once

#ifndef NDEBUG
#include "img/io.hpp"
#endif

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
  static constexpr pxidx_t hh = (T::RowsAtCompileTime >> 1);
  static constexpr pxidx_t hw = (T::ColsAtCompileTime >> 1);
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
  pure auto operator()(pxidx_t i, pxidx_t j) const noexcept -> u8 { return array(i, j); }
  pure auto operator[](px_t const& ij) const noexcept -> u8 { return operator()(std::get<0>(ij), std::get<1>(ij)); }
  pure auto find_imprecise(px_t const& ij) const NOX->pxdiff_t;
  pure auto find_precise(px_t const& ij) const NOX->pxdiff_t;  // still O(lg n)
};

// TODO(wrsturgeon): pass # recursive calls as an argument so we don't have to left-shift by one each time
template <imsize_t H, imsize_t W>
pure auto
Pyramid<H, W>::find_imprecise(px_t const& ij) const NOX->pxdiff_t {
  auto const [i, j] = ij;
  if (i >= H) { return (j >= W) ? pxdiff_t{-1, -1} : pxdiff_t{-1, 0}; }
  if (j >= W) { return {0, -1}; }
  if (array(i, j) >= kPyramidFindThreshold) { return {0, 0}; }
  bool const lsafe = (j >= 1);
  bool const rsafe = (j < (W - 1));
  bool const usafe = (i >= 1);
  bool const dsafe = (i < (H - 1));
  // This section is incredibly verbose, redundant, and going to lead to a marginally longer executable file, but each execution will be as fast as possible
  if (usafe) {
    if (array(i - 1, j) >= kPyramidFindThreshold) { return {-1, 0}; }
    if (lsafe) {
      if (array(i, j - 1) >= kPyramidFindThreshold) { return {0, -1}; }
      if (dsafe) {
        if (array(i + 1, j) >= kPyramidFindThreshold) { return {1, 0}; }
        if (rsafe) {  // All 4 directions are safe
          if (array(i, j + 1) >= kPyramidFindThreshold) { return {0, 1}; }
          if (array(i - 1, j - 1) >= kPyramidFindThreshold) { return {-1, -1}; }
          if (array(i + 1, j - 1) >= kPyramidFindThreshold) { return {1, -1}; }
          if (array(i - 1, j + 1) >= kPyramidFindThreshold) { return {-1, 1}; }
          if (array(i + 1, j + 1) >= kPyramidFindThreshold) { return {1, 1}; }
        } else {  // Only right (j + 1) is off the map
          if (array(i - 1, j - 1) >= kPyramidFindThreshold) { return {-1, -1}; }
          if (array(i + 1, j - 1) >= kPyramidFindThreshold) { return {1, -1}; }
        }
      } else {        // Down (i + 1) is off the map
        if (rsafe) {  // Only down (i + 1) is off the map
          if (array(i, j + 1) >= kPyramidFindThreshold) { return {0, 1}; }
          if (array(i - 1, j + 1) >= kPyramidFindThreshold) { return {-1, 1}; }
          if (array(i - 1, j - 1) >= kPyramidFindThreshold) { return {-1, -1}; }
        } else {  // Down (i + 1) and right (j + 1) are off the map
          if (array(i - 1, j - 1) >= kPyramidFindThreshold) { return {-1, 1}; }
        }
      }
    } else {  // Left (j - 1) is off the map
      if (dsafe) {
        if (array(i + 1, j) >= kPyramidFindThreshold) { return {1, 0}; }
        if (rsafe) {  // Only left (j - 1) is off the map
          if (array(i, j + 1) >= kPyramidFindThreshold) { return {0, 1}; }
          if (array(i + 1, j + 1) >= kPyramidFindThreshold) { return {1, 1}; }
          if (array(i - 1, j + 1) >= kPyramidFindThreshold) { return {-1, 1}; }
        }       // Left (j - 1) and right (j + 1) are off the map: do nothing
      } else {  // Down (i + 1) and left (j - 1) are off the map
        if (rsafe) {
          if (array(i, j + 1) >= kPyramidFindThreshold) { return {0, 1}; }
          if (array(i - 1, j + 1) >= kPyramidFindThreshold) { return {-1, 1}; }
        }  // Down (i + 1), left (j - 1), and right (j + 1) are off the map: do nothing
      }
    }
  } else {  // Up (i - 1) is off the map
    if (lsafe) {
      if (array(i, j - 1) >= kPyramidFindThreshold) { return {0, -1}; }
      if (dsafe) {
        if (array(i + 1, j) >= kPyramidFindThreshold) { return {1, 0}; }
        if (rsafe) {  // Only up (i - 1) is off the map
          if (array(i, j + 1) >= kPyramidFindThreshold) { return {0, 1}; }
          if (array(i + 1, j + 1) >= kPyramidFindThreshold) { return {1, 1}; }
          if (array(i + 1, j - 1) >= kPyramidFindThreshold) { return {1, -1}; }
        } else {  // Up (i - 1) and right (j + 1) are off the map
          if (array(i + 1, j - 1) >= kPyramidFindThreshold) { return {1, -1}; }
        }
      } else {  // Down (i + 1) and up (i - 1) are off the map
        if (rsafe) {
          if (array(i, j + 1) >= kPyramidFindThreshold) { return {0, 1}; }
        }  // Down (i + 1), up (i - 1), and right (j + 1) are off the map: do nothing
      }
    } else {  // Up (i - 1) and left (j - 1) are off the map
      if (dsafe) {
        if (array(i + 1, j) >= kPyramidFindThreshold) { return {1, 0}; }
        if (rsafe) {  // Only up (i - 1) and left (j - 1) are off the map
          if (array(i, j + 1) >= kPyramidFindThreshold) { return {0, 1}; }
          if (array(i + 1, j + 1) >= kPyramidFindThreshold) { return {1, 1}; }
        }       // Up (i - 1), left (j - 1), and right (j + 1) are off the map: do nothing
      } else {  // Down (i + 1), up (i - 1), and left (j - 1) are off the map
        if (rsafe) {
          if (array(i, j + 1) >= kPyramidFindThreshold) { return {0, 1}; }
        }  // Down (i + 1), up (i - 1), left (j - 1), and right (j + 1) are off the map: do nothing
      }
    }
  }
  if constexpr ((H > 1) and (W > 1)) {
    auto [ii, jj] = dn.find_imprecise({i >> 1, j >> 1});
    return {ii << 1, jj << 1};
  }
#ifndef NDEBUG
  throw std::runtime_error{"Pyramid::find... failed: no above-threshold value found"};
#endif
  return {0, 0};
}

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
