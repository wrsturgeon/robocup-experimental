#pragma once

#include "rnd/xoshiro.hpp"

#include <array>    // std::array
#include <cstddef>  // std::size_t
#include <memory>   // std::unique_ptr
#include <utility>  // std::move, std::swap

namespace training {

static constexpr uint8_t kDefaultABits = 12;
static constexpr uint8_t kMaxABits = 16;

/**
 * Mechanism for randomizing access to training data to temper recency bias.
 * WARNING: Not responsible for initializing, maintaining, or freeing data!
 */
template <typename T, uint8_t abits = kDefaultABits> class Scrambler {
 public:
  Scrambler();
  void store_only(T&& current);  // Call EXACTLY 1<<abits times, then use store_and_recall
  auto store_and_recall(T&& current) -> T;  // Call store_only 1<<abits times before this
 private:
  static_assert(abits, "Scrambler abits can't be 0");
  static_assert(abits <= kMaxABits, "No way you'll need this many memories at a time");
  static constexpr std::size_t n = static_cast<std::size_t>(1) << abits;
  static_assert(n, "Scrambler abits is too big for this OS");
  static constexpr uint8_t n_renew = kSystemBits / abits;  // Number of uses per xoshiro result
  static constexpr rnd::t bitmask = n - 1;
  rnd::t rnd_state;
  uint8_t rnd_uses_left{0};
  std::array<T, n> data;
  unsigned counter : abits{0};
};

template <typename T, uint8_t abits>
Scrambler<T, abits>::Scrambler() : rnd_state{uninitialized<rnd::t>()} {}

template <typename T, uint8_t abits>
void
Scrambler<T, abits>::store_only(T&& current) {
  data[counter++] = std::move(current);
}

template <typename T, uint8_t abits>
auto
Scrambler<T, abits>::store_and_recall(T&& current) -> T {
  if (!rnd_uses_left) {
    rnd_uses_left = n_renew - 1;
    rnd_state = rnd::next();
  } else {
    --rnd_uses_left;
  }
  rnd::t rndidx = rnd_state & bitmask;
  rnd_state >>= abits;
  std::swap(data[rndidx], current);
  return std::move(current);
}

}  // namespace training
