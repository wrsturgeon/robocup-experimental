#ifndef QBIT_TYPES_HPP_
#define QBIT_TYPES_HPP_

#include <stddef.h>
#include <type_traits>

namespace qbit {

static constexpr size_t mask_msb(size_t);

struct t {
  unsigned v : 4;
};

template <size_t n>
struct field {
  unsigned data : n << 2;
};

template <size_t n>
class v {
public:
  static_assert(!(n & (n - 1)), "n must be a power of 2");
  static constexpr size_t bits = n << 2;
  static_assert(sizeof(v<n>) == bits, "Instantiation of qbit::v occupies more bits than expected");
  static constexpr size_t clear_msb = mask_msb(n);
  inline __attribute__((always_inline)) t operator[](size_t i) const;

protected:
  field<n> internal;
};

template <size_t w, size_t h>
class m {
public:
  v<w> row[h];
  void downsample(m<(w >> 1), (h >> 1)>& dst) const;
};

} // namespace qbit

#endif // QBIT_TYPES_HPP_
