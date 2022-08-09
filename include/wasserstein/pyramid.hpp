#pragma once

#include <string.h>

#include <iostream>

#include "eigen.hpp"
#include "rnd/xoshiro.hpp"
#include "vision/image-api.hpp"
#include "vision/pxpos.hpp"

namespace wasserstein {

template <vision::pxidx_t w, vision::pxidx_t h>
using EigenMap = Eigen::Map<Eigen::Matrix<uint8_t, h, w, Eigen::RowMajor>>;

inline static constexpr size_t pyrsize(vision::pxidx_t w, vision::pxidx_t h);  // NOLINT(clang-diagnostic-unneeded-internal-declaration)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-length-array"

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
  uint8_t _array[h][w];          // NOT uint8_t**, thankfully--contiguous row-major format
  uint8_t _up_raw[bytes_above];  // Please never access directly: use up()
  Pyramid(uint8_t* const __restrict src);

 public:
  Pyramid() = delete;
  Pyramid(Pyramid const&) = delete;
  Pyramid(uint8_t src[h][w]);
  Pyramid(vision::NaoImage<w, h> const& src);
  uint8_t& operator()(vision::pxidx_t x, vision::pxidx_t y);
  up_t& up();
  // The coolest thing is that it doesn't even matter if we call up() one or two or n times too many--
  // it'll still return the same 0-element Pyramid!
  // We just need some kind of minimal (preferably compile-time) bounds checking in public methods
};

#pragma clang diagnostic pop

}  // namespace wasserstein
