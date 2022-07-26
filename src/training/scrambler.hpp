#if TRAINING_ENABLED
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
  Scrambler() : rnd_uses_left{0} {}
  MEMBER_INLINE T const* store_and_recall(T const *const current); // THIS CAN AND WILL BE NULL FOR THE FIRST (1<<abits) CALLS
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



/**
 * WARNING: CAN AND WILL BE NULL
 * Randomly indexes, replaces that entry, and returns the older one.
 */
template <typename T, uint8_t abits>
MEMBER_INLINE T const* Scrambler<T, abits>::store_and_recall(T const *const current) {
  if (!rnd_uses_left) {
    rnd_uses_left = n_renew - 1;
    rnd_state = rnd::next();
  } else { --rnd_uses_left; }
  rnd::t rndidx = rnd_state & bitmask;
  rnd_state >>= abits;
  T const *const tmp = data[rndidx];
  data[rndidx] = current;
  return tmp;
}

} // namespace training

#endif // TRAINING_SCRAMBLER_HPP_

#else // TRAINING_ENABLED
#pragma message("Skipping scrambler.hpp; training module disabled")
#endif // TRAINING_ENABLED
