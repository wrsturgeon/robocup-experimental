#if UTIL_ENABLED
#ifndef UTIL_DUFF_HPP_
#define UTIL_DUFF_HPP_

// Duff's device: https://en.wikipedia.org/wiki/Duff%27s_device
// I'm not sure this would actually provide any benefit--
// let's benchmark it when we have some time to spare

#include <stddef.h> // size_t

namespace util {



#define DUFF(stmt, times)       \
size_t i = ((times) + 7) >> 3;  \
switch (i & 7) do {             \
          stmt;                 \
  case 7: stmt;                 \
  case 6: stmt;                 \
  case 5: stmt;                 \
  case 4: stmt;                 \
  case 3: stmt;                 \
  case 2: stmt;                 \
  case 1: stmt;                 \
  case 0:     ;                 \
} while (--i)



} // namespace util

#endif // UTIL_DUFF_HPP_

#else // UTIL_ENABLED
#pragma message("Skipping duff.hpp; util module disabled")
#endif // UTIL_ENABLED
