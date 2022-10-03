#pragma once

template <auto t1f2, typename T>
[[nodiscard]] INLINE static constexpr auto ifc(T v_if_true, T v_if_false) -> T {
  if constexpr (t1f2) {
    return v_if_true;
  } else {
    return v_if_false;
  }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
// NOLINTBEGIN(misc-unused-parameters)
[[nodiscard]] INLINE static constexpr auto if32(auto val_if_32, auto val_if_64) -> decltype(auto) {
#if BITS == 32
  return val_if_32;
#elif BITS == 64
  return val_if_64;
#else
#error "Invalid value for BITS"
#endif
}
// NOLINTEND(misc-unused-parameters)
#pragma clang diagnostic pop
