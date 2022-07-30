#ifndef TRAINING_SCRAMBLER_HPP_
#define TRAINING_SCRAMBLER_HPP_

#include <stddef.h> // size_t

#include "rnd/xoshiro.hpp"

namespace training {



/**
 * Mechanism for randomizing access to training data to temper recency bias.
 * WARNING: Not responsible for initializing, maintaining, or freeing data!
 */
template <typename T, uint8_t abits = 12>
class Scrambler {
public:
  Scrambler(Scrambler const&) = delete;
  Scrambler();
  inline T const* store_and_recall(T const *const current); // THIS CAN AND WILL BE NULL
protected:
  static_assert(abits, "Scrambler abits can't be 0");
  static constexpr size_t n = static_cast<size_t>(1) << abits;
  static_assert(n, "Scrambler abits is too big for this OS");
  static constexpr uint8_t n_renew = BITS / abits; // Number of times we can use xoshiro result
  static constexpr rnd::t bitmask = n - 1;
  rnd::t rnd_state;
  uint8_t rnd_uses_left;
  T const *const data[n];
};



} // namespace training

#endif // TRAINING_SCRAMBLER_HPP_
