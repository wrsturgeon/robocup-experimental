#ifndef QBIT_TYPES_HPP_
#define QBIT_TYPES_HPP_

#include "vcl.hpp"

#include <iostream>

namespace qbit {

class t {
private:
  unsigned value : 4;

public:
  t(uint8_t v);
  friend std::ostream& operator<<(std::ostream& os, t const& x);
};

template <size_t n>
class v {
protected:
  // 8 4-bit ints per uint32_t
  uint32_t full[n >> 3];
  uint32_t part : (n & 7);

public:
  uint8_t operator[](size_t i) const;
};

} // namespace qbit

#endif // QBIT_TYPES_HPP_
