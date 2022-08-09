#include "training/scrambler.hpp"

namespace training {

template <typename T, uint8_t abits>
Scrambler<T, abits>::Scrambler() : rnd_state{uninitialized<rnd::t>()}, rnd_uses_left{0}, counter{0} {}

template <typename T, uint8_t abits>
void Scrambler<T, abits>::store_only(T&& current) {
  data[counter++] = std::move(current);
}

template <typename T, uint8_t abits>
auto Scrambler<T, abits>::store_and_recall(T&& current) -> T {
  if (rnd_uses_left == 0) {
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
