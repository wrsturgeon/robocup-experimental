#include "wasserstein/pyramid.hpp"

namespace wasserstein {

using Eigen::seqN;
using Eigen::placeholders::all;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-length-array"

// TODO(wrsturgeon): consteval when clang-tidy implements it
inline static constexpr auto pyrsize(vision::pxidx_t w, vision::pxidx_t h) -> size_t {  // NOLINT(misc-no-recursion)
  return ((w != 0) && (h != 0))
               ? static_cast<size_t>(w * h) + pyrsize(
                                                    static_cast<vision::pxidx_t>(w >> 1),
                                                    static_cast<vision::pxidx_t>(h >> 1))
               : 0;
}

#define PYRAMID_ERROR "Pyramid class must have only 2 allocated members: _array & _up_raw"  // NOLINT(cppcoreguidelines-macro-usage)

template <vision::pxidx_t w, vision::pxidx_t h>
Pyramid<w, h>::Pyramid(uint8_t const* const __restrict src) : _array{uninitialized(_array)},
                                                              _up_raw{uninitialized(_up_raw)} {
  // Private constructor: guaranteed that by this point, src points to valid (w, h) uint8_t data
  static_assert(sizeof(Pyramid<0, 0>) == 0, PYRAMID_ERROR);
  static_assert(sizeof(Pyramid<1, 1>) == 1, PYRAMID_ERROR);
  // memcpy(_array, src, n_px);
  // build();
  EigenMap<w, h> input_map{src};
  EigenMap<w, h> internal_map{&_array[0][0]};
  internal_map = ~input_map;  // 255 -> 0, 254 -> 1, ..., 0 -> 255
  build_eigen(internal_map);
}

template <vision::pxidx_t w, vision::pxidx_t h>
Pyramid<w, h>::Pyramid(uint8_t src[h][w]) : Pyramid{src} {}  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)

template <vision::pxidx_t w, vision::pxidx_t h>
Pyramid<w, h>::Pyramid(vision::NaoImage<w, h> const& src) : Pyramid{src.internal.data()} {}

template <vision::pxidx_t w, vision::pxidx_t h>  // NOLINT(fuchsia-overloaded-operator)
auto Pyramid<w, h>::operator()(vision::pxidx_t x, vision::pxidx_t y) -> uint8_t& {
  return _array[y][x];
}

template <vision::pxidx_t w, vision::pxidx_t h>
auto Pyramid<w, h>::up() -> up_t& {
  return *reinterpret_cast<up_t*>(_up_raw);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
  // The coolest thing is that it doesn't even matter if we call up() one or two or n times too many--
  // it'll still return the same 0-element Pyramid!
  // We just need some kind of minimal (preferably compile-time) bounds checking in public methods
  // And keep in mind that 0-element arrays are a compiler extension
}

template <vision::pxidx_t w, vision::pxidx_t h>
void Pyramid<w, h>::build_manual() {
  // Assumes we've already filled `_array` with valid image data

  static_assert(sizeof(Pyramid<w, h>) == pyrsize(w, h), PYRAMID_ERROR);

  // NOTE: AN INFINITE CONSTRUCTOR LOOP DOES *NOT* THROW A COMPILE-TIME ERROR
  // Need to verify that we have > 0 pixels to fill, else infinite loop of constructing nothing
  if ((half_w == 0) || (half_h == 0)) {
    return;
  }

  // Allocating for use in loops
  auto twice = uninitialized<vision::pxidx_t>();
  auto a = uninitialized<uint8_t>();
  auto b = uninitialized<uint8_t>();

  // TODO(wrsturgeon): randomize shift when odd size

  // One dimension at a time: first half the width
  uint8_t tmp[h][half_w];  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  for (vision::pxidx_t y = 0; y < h; ++y) {
    for (vision::pxidx_t x = 0; x < half_w; ++x) {
      twice = static_cast<vision::pxidx_t>(x << 1);  // TODO(wrsturgeon): verify no overflow
      a = _array[y][twice];
      b = _array[y][twice | 1];
      tmp[y][x] = ((a >> 2) > (b >> 2)) ? a & ~1 : b | 1;  // Set a bit to represent left/right provenience (thx Sydney)
    }
  }

  // And half that, directly into the next layer
  Pyramid<half_w, half_h>& dst = up();
  for (vision::pxidx_t y = 0; y < half_h; ++y) {
    for (vision::pxidx_t x = 0; x < half_w; ++x) {
      twice = static_cast<vision::pxidx_t>(y << 1);  // TODO(wrsturgeon): verify no overflow
      a = tmp[twice][x];
      b = tmp[twice | 1][x];
      dst(x, y) = ((a >> 2) > (b >> 2)) ? a & ~2 : b | 2;
    }
  }

  dst.build_manual();
}

template <vision::pxidx_t w, vision::pxidx_t h>
void Pyramid<w, h>::build_eigen(EigenMap<w, h> const& lower) {
  static_assert((half_w != 0) && (half_h != 0), "Pyramid level to be constructed must have at least one pixel");
  static_assert(sizeof(Pyramid<w, h>) == pyrsize(w, h), PYRAMID_ERROR);
  auto i0 = uninitialized<uint8_t>();
  Pyramid<half_w, half_h>& next = up();

  // Eigen has some obsessive beef with things that look like column vectors but aren't
  if constexpr (half_w == 1) {
    auto upper_map = Eigen::Map<Eigen::Vector<uint8_t, half_h>>{next._array};
    if constexpr ((w & 1) == 0) {
      i0 = 0;
    } else {
      i0 = static_cast<uint8_t>(rnd::bit());
    }
    auto tmp = (lower(all, seqN(i0, half_w, 2)) >> 1) + (lower(all, seqN(i0 + 1, half_w, 2)) >> 1);
    if constexpr ((h & 1) == 0) {
      i0 = 0;
    } else {
      i0 = static_cast<uint8_t>(rnd::bit());
    }
    upper_map = (tmp(seqN(i0, half_h, 2), all) >> 1) + (tmp(seqN(i0 + 1, half_h, 2), all) >> 1);
  } else {  // Literally anything else
    auto upper_map = EigenMap<half_w, half_h>{next._array};
    if constexpr ((w & 1) == 0) {
      i0 = 0;
    } else {
      i0 = static_cast<uint8_t>(rnd::bit());
    }
    auto tmp = (lower(all, seqN(i0, half_w, 2)) >> 1) + (lower(all, seqN(i0 + 1, half_w, 2)) >> 1);
    if constexpr ((h & 1) == 0) {
      i0 = 0;
    } else {
      i0 = static_cast<uint8_t>(rnd::bit());
    }
    upper_map = (tmp(seqN(i0, half_h, 2), all) >> 1) + (tmp(seqN(i0 + 1, half_h, 2), all) >> 1);
    if constexpr (half_h != 1) {
      next.build_eigen(upper_map);
    }
  }
}

#pragma clang diagnostic pop

}  // namespace wasserstein
