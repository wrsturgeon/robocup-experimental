#if VISION_ENABLED
#ifndef VISION_PYRAMID_HPP_
#define VISION_PYRAMID_HPP_

#include <stdint.h>

#include <eigen.hpp>

#include <vision/image_api.hpp>

namespace vision {



// Forward declaration
template <pxidx_t w, pxidx_t h>
class Pyramid;



/**
 * Data structure for position-wise Wasserstein ("earth-mover") distance in an image.
 * Takes a pixel value and a location, then estimates how far you'd have to go to be right.
 */
template <pxidx_t w, pxidx_t h>
class Pyramid {
public:
  Pyramid() { for (uint8_t i = 0; i < n_layers; ++i) std::construct_at(layer<i>()); }
  ~Pyramid() { for (uint8_t i = 0; i < n_layers; ++i) std::destroy_at(layer<i>()); }
protected:
  template <uint8_t i> class Layer;
  template <uint8_t i> MEMBER_INLINE constexpr Layer<i>* layer() const;
  template <uint8_t i> MEMBER_INLINE void set_layer(Layer<i> const& layer);
  static constexpr uint8_t n_layers = util::lgp1((h > w) ? h : w);

  // Raw byte array--please never directly access
  // See https://en.cppreference.com/w/cpp/memory/construct_at
  uint8_t _storage[Layer<n_layers>::index()];
};



/**
 * Layer in an image-pyramid of height (w, h)
 * Subclasses Eigen::Matrix<uint8_t, w, h, RowMajor>
 * Only one channel (for our purposes, lumped difference)
 * Sole addition to the class is an `index` method
 *   that gives its index in bytes, assuming
 *   each non-base layer is after the one immediately larger.
 */
template <pxidx_t w, pxidx_t h>
template <uint8_t i>
class Pyramid<w, h>::Layer : public Eigen::Matrix<uint8_t, (w >> i), (h >> i), Eigen::StorageOptions::RowMajor> {
public:
  using Eigen::Matrix<uint8_t, (w >> i), (h >> i), Eigen::StorageOptions::RowMajor>::Matrix;
  INLINE constexpr size_t index() {
    using last_t = Layer<i - 1>; // recursively
    return i ? sizeof(last_t) + last_t::index() : 0;
  }
};



/**
 *  C++ arrays mandate that each element have the same type, even template arguments.
 *  But for this class we need a (variable, but compile-time)-sized array of layers,
 *    each of which has a different size (as a compile-time template argument).
 *  So we do what we would normally do in C:
 *    whip up an uninitialized byte array of known size,
 *    then initialize it after the fact (but ASAP).
 *  I initially thought `Eigen::Matrix` instances would all be the same size
 *    (i.e. initialize their data on the heap), but that's not the case;
 *    however, this still works perfectly, since the _offset_ in bytes
 *    is known a priori (add the last element's size to the last offset, recursively).
 */
template <pxidx_t w, pxidx_t h>
template <uint8_t i>
MEMBER_INLINE constexpr typename Pyramid<w, h>::template Layer<i>* Pyramid<w, h>::layer() const {
  static_assert(i < n_layers, "Pyramid::layer(): i out of range");
  return reinterpret_cast<Layer<i>*>(_storage + Layer<i>::index());
}



template <pxidx_t w, pxidx_t h>
template <uint8_t i>
MEMBER_INLINE void Pyramid<w, h>::set_layer(Pyramid<w, h>::Layer<i> const& layer) {
  static_assert(i < n_layers, "Pyramid::set_layer(): i out of range");
  this->template layer<i>()->operator=(layer);
}



} // namespace vision

#endif // VISION_PYRAMID_HPP_

#else // VISION_ENABLED
#pragma message("Skipping pyramid.hpp; vision module disabled")
#endif // VISION_ENABLED
