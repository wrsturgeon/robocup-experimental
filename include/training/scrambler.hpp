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
template <typename T, uint8_t abits = kDefaultABits>
class Scrambler {
 public:
  Scrambler();
  void store_only(T&& current);             // Call EXACTLY 1<<abits times, then use store_and_recall
  auto store_and_recall(T&& current) -> T;  // WARNING: Use `store_only` EXACTLY 1<<abits times before calling this!
 private:
  static_assert(abits, "Scrambler abits can't be 0");
  static_assert(abits <= kMaxABits, "No fucking way you're going to need that many memories at a time");
  static constexpr std::size_t n = static_cast<std::size_t>(1) << abits;
  static_assert(n, "Scrambler abits is too big for this OS");
  static constexpr uint8_t n_renew = BITS / abits;  // Number of times we can use xoshiro result
  static constexpr rnd::t bitmask = n - 1;
  rnd::t rnd_state;
  uint8_t rnd_uses_left;
  std::array<T, n> data;
  unsigned counter : abits;
};

}  // namespace training
