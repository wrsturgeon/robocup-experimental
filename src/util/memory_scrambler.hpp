#if TRAINING

#ifndef UTIL_MEMORY_SCRAMBLER_HPP_
#define UTIL_MEMORY_SCRAMBLER_HPP_

#include <stdint.h>

#include <measurement/units.hpp>
#include <util/xoshiro.hpp>
#include <vision/image_api.hpp>



struct Memory {
  Position nao_pos, ball_pos;
  NaoImage img;
};



/**
 * Mechanism for randomizing access to training data to temper recency bias.
 * WARNING: Not responsible for initializing, maintaining, or freeing data!
 */
template <uint8_t abits = 12>
class Scrambler {
public:
  Scrambler(Scrambler const&) = delete;
  Scrambler() : rnd_uses_left{0} {}
  Memory const *const store_and_recall(Memory const *const current); // THIS CAN AND WILL BE NULL FOR THE FIRST (1<<abits) CALLS
protected:
  static_assert(abits, "Scrambler abits can't be 0");
  static constexpr size_t n = static_cast<size_t>(1) << abits;
  static_assert(n, "Scrambler abits is too big for this OS");
  static constexpr uint8_t n_renew = BITS / abits; // Number of times we can use xoshiro result
  static constexpr rnd::t bitmask = n - 1;
  rnd::t rnd_persistent;
  uint8_t rnd_uses_left;
  Memory const *const data[n];
};



/**
 * WARNING: CAN AND WILL BE NULL
 * Randomly indexes, replaces that entry, and returns the older one.
 */
template <uint8_t abits>
Memory const *const Scrambler<abits>::store_and_recall(Memory const *const current) {
  if (!rnd_uses_left) {
    rnd_uses_left = n_renew - 1;
    rnd_persistent = rnd::next();
  } else { --rnd_uses_left; }
  rnd::t rndidx = rnd_persistent & bitmask;
  rnd_persistent >>= abits;
  Memory const *const tmp = data[rndidx];
  data[rndidx] = current;
  return tmp;
}



#endif // UTIL_MEMORY_HPP_

#endif // TRAINING
