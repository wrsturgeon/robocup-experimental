#pragma once

template <auto t1f2>
pure static auto
ifc(auto const& v_if_true, auto const& v_if_false) -> decltype(auto) {
  if constexpr (t1f2) {
    return v_if_true;
  } else {
    return v_if_false;
  }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
pure static auto
if32(auto val_if_32, auto val_if_64) -> decltype(auto) {  // NOLINT(misc-unused-parameters)
#if BITS == 32
  return val_if_32;
#else
  return val_if_64;
#endif
}
#pragma clang diagnostic pop
