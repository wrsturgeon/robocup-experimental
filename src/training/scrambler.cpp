#include "training/scrambler.hpp"

namespace training {

template <typename T, uint8_t abits>
Scrambler<T, abits>::Scrambler() : rnd_uses_left{0} {}

template <typename T, uint8_t abits>
std::unique_ptr<T const>
Scrambler<T, abits>::store_and_recall(std::unique_ptr<T const>& current) {
  if (!rnd_uses_left) {
    rnd_uses_left = n_renew - 1;
    rnd_state = rnd::next();
  } else {
    --rnd_uses_left;
  }
  rnd::t rndidx = rnd_state & bitmask;
  rnd_state >>= abits;
  std::unique_ptr<T const> tmp = std::move(data[rndidx]); // Get the data that's currently at the
  data[rndidx] = std::move(current);
  return std::move(tmp);
}

} // namespace training
