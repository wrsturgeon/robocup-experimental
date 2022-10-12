#pragma once

template <auto t1f2, typename T>
pure static auto ifc(T v_if_true, T v_if_false) -> T {
  if constexpr (t1f2) {
    return v_if_true;
  } else {
    return v_if_false;
  }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
// NOLINTNEXTLINE(misc-unused-parameters)
pure static auto if32(auto val_if_32, auto val_if_64) -> decltype(auto) {
#if BITS == 32
  return val_if_32;
#else
  return val_if_64;
#endif
}
#pragma clang diagnostic pop
