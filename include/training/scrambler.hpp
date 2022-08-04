#ifndef TRAINING_SCRAMBLER_HPP_
#define TRAINING_SCRAMBLER_HPP_

#include "rnd/xoshiro.hpp"

#include <memory>   // std::unique_ptr
#include <stddef.h> // size_t

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
  std::unique_ptr<T const> store_and_recall(std::unique_ptr<T const>& current); // THIS CAN AND WILL BE NULL
protected:
  static_assert(abits, "Scrambler abits can't be 0");
  static_assert(abits <= 16, "No fucking way you're going to need more than 65,536 memories at a time");
  static constexpr size_t n = static_cast<size_t>(1) << abits;
  static_assert(n, "Scrambler abits is too big for this OS");
  static constexpr uint8_t n_renew = BITS / abits; // Number of times we can use xoshiro result
  static constexpr rnd::t bitmask = n - 1;
  rnd::t rnd_state;
  uint8_t rnd_uses_left;
  std::unique_ptr<T const> data[n];
};

} // namespace training

#endif // TRAINING_SCRAMBLER_HPP_
