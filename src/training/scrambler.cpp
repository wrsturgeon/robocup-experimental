#include "training/scrambler.hpp"

namespace training {



template <typename T, uint8_t abits>
Scrambler<T, abits>::Scrambler() : rnd_uses_left{0} {}



template <typename T, uint8_t abits>
T const* Scrambler<T, abits>::store_and_recall(T const *const current) {
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
