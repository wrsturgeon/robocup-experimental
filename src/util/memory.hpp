#if TRAINING

#ifndef UTIL_MEMORY_HPP_
#define UTIL_MEMORY_HPP_

#include <stdint.h>

#include <measurement/units.hpp>
#include <util/xoshiro.hpp>
#include <vision/image_api.hpp>



struct Memory {
  pos_t pos;
  NaoImage img;
};



/**
 * Mechanism for randomizing access to training data to temper recency bias.
 */
template <uint8_t abits = 12>
class Mixer {
public:
  Mixer(Mixer const&) = delete;
  Mixer() : rndidx{rnd::next()} {}
  Memory* store_and_recall(Memory const& current); // THIS CAN AND WILL BE NULL FOR THE FIRST (1<<abits) CALLS
protected:
  static_assert(abits, "Mixer abits can't be 0");
  static constexpr size_t n = static_cast<size_t>(1) << abits;
  static_assert(n, "Mixer abits is too big for this OS");
  static constexpr uint8_t n_renew = BITS / abits; // Number of times we can use xoshiro result
  static constexpr rnd::t bitmask = n - 1;
  rnd::t rndidx;
  uint8_t rndidx_uses_left = n_renew;
};



/**
 * WARNING: CAN AND WILL BE NULL FAIRLY OFTEN!
 * Randomly chooses an index, copies (to return), and replaces at that index.
 */
Memory& Mixer::store_and_recall(Memory const& current) {
  asdf
}



#endif // UTIL_MEMORY_HPP_

#endif // TRAINING
