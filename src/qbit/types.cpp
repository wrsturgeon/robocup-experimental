#include "qbit/types.hpp"

namespace qbit {

static constexpr size_t
mask_msb(size_t qbits) {
  return qbits ? 0x7 + (mask_msb(qbits - 1) << 4) : 0;
}

template <size_t n>
t
v<n>::operator[](size_t i) const {
  return (internal >> (i << 2)) & 0xf;
}

template <size_t w, size_t h>
void
m<w, h>::downsample(m<(w >> 1), (h >> 1)>& dst) const {
  for (v<w>& r : this->row) {
    field<w> half_each = (r.internal >> 1) & r.clear_msb; // Works only on UNSIGNED data
    dst.internal = (half_each + (half_each >> 4)) & this->every_other;
  }
}

} // namespace qbit