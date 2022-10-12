#pragma once

#include "rnd/xoshiro.hpp"

#include "util/uninitialized.hpp"

#include <array>    // std::array
#include <bit>      // std::bit_width
#include <cassert>  // assert
#include <cstddef>  // std::size_t
#include <memory>   // std::unique_ptr
#include <utility>  // std::move, std::swap

namespace ml {

inline constexpr std::uint8_t kDefaultABits = 12;
inline constexpr std::uint8_t kMaxABits = 16;

/**
 * Mechanism for randomizing access to ml data to temper recency bias.
 * WARNING: Not responsible for initializing, maintaining, or freeing data!
 */
template <typename T, std::uint8_t abits = kDefaultABits>
class Scrambler {
 public:
  INLINE void prefill(T&& current);                       // Call EXACTLY 1<<abits times, then use store_and_recall
  [[nodiscard]] auto store_and_recall(T&& current) -> T;  // Call store_only 1<<abits times before this
 private:
  static_assert(abits, "Scrambler abits can't be 0");
  static_assert(abits <= kMaxABits, "No way you'll need this many memories at a time");
  static constexpr std::size_t n = std::size_t{1} << abits;
  static_assert(n, "Scrambler abits is too big for this OS");
  static constexpr std::uint8_t n_renew = kSystemBits / abits;  // # of uses per xoshiro result
  static constexpr rnd::t bitmask = n - 1;
  std::uint8_t rnd_uses_left = n_renew - 1;
  rnd::t rnd_state = rnd::next();
  std::array<T, n> data = uninitialized<std::array<T, n>>();
  cint<std::bit_ceil(abits), unsigned> counter = 0;
#ifndef NDEBUG
  std::size_t n_prefilled = 0;  // NOLINT(clang-diagnostic-padded)
#endif
};

template <typename T, std::uint8_t abits>
INLINE void Scrambler<T, abits>::prefill(T&& current) {
  data[counter++] = std::move(current);
  assert(std::bit_width(n_prefilled++) <= abits);  // WHY THE FUCK DOES THIS STOP COVERAGE WHEN IT COMES FIRST
}

template <typename T, std::uint8_t abits>
auto Scrambler<T, abits>::store_and_recall(T&& current) -> T {
  assert(n_prefilled == n);
  if (!rnd_uses_left) {
    rnd_uses_left = n_renew - 1;
    rnd_state = rnd::next();
  } else {
    --rnd_uses_left;
  }
  rnd::t const rndidx = rnd_state & bitmask;
  rnd_state >>= abits;
  std::swap(data[rndidx], current);
  return std::move(current);
}

}  // namespace ml
