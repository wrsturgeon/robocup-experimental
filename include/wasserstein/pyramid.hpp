#pragma once

#include "rnd/xoshiro.hpp"
#include "vision/image-api.hpp"
#include "vision/pxpos.hpp"

#include "eigen.hpp"

#include <iostream>
#include <string>

namespace wasserstein {

template <vision::pxidx_t w, vision::pxidx_t h>
using EigenMap = Eigen::Map<Eigen::Matrix<uint8_t, h, w, Eigen::RowMajor>>;

inline static constexpr auto pyrsize(vision::pxidx_t w, vision::pxidx_t h) -> size_t;  // NOLINT(clang-diagnostic-unneeded-internal-declaration)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-length-array"

// Forward declaration to use in sizing memory below
inline static constexpr auto pyrsize(vision::pxidx_t w, vision::pxidx_t h) -> size_t;  // NOLINT(clang-diagnostic-unneeded-internal-declaration)

template <vision::pxidx_t w, vision::pxidx_t h>
class Pyramid {
 protected:
  template <vision::pxidx_t w_, vision::pxidx_t h_>
  friend class Pyramid;
  static constexpr uint32_t n_px = w * h;
  static constexpr vision::pxidx_t half_w = w >> 1;
  static constexpr vision::pxidx_t half_h = h >> 1;
  static constexpr size_t bytes_above = pyrsize(half_w, half_h);  // C++ doesn't like substituting this directly
  using up_t = Pyramid<half_w, half_h>;
  void build_manual();                                // Max-pooling (directional flags)
  void build_eigen(EigenMap<w, h> const& lower_map);  // Mean-pooling (automatic)
 private:
  uint8_t _array[h][w];          // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  uint8_t _up_raw[bytes_above];  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  explicit Pyramid(uint8_t const* __restrict src);

 public:
  explicit Pyramid(uint8_t src[h][w]);  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  explicit Pyramid(vision::NaoImage<w, h> const& src);
  auto operator()(vision::pxidx_t x, vision::pxidx_t y) -> uint8_t&;  // NOLINT(fuchsia-overloaded-operator)
  auto up() -> up_t&;
  // The coolest thing is that it doesn't even matter if we call up() one or two or n times too many--
  // it'll still return the same 0-element Pyramid!
  // We just need some kind of minimal (preferably compile-time) bounds checking in public methods
};

#pragma clang diagnostic pop

}  // namespace wasserstein
