#pragma once

template <auto val_if_32, auto val_if_64>
static constexpr inline __attribute__((always_inline)) auto
if32() -> auto{
  if constexpr (kSystemBits == 32) {
    return val_if_32;
  } else {
    return val_if_64;
  }
}
