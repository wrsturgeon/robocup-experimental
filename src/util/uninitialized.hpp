#pragma once

#include <utility>  // std::move

template <typename T>
inline __attribute__((always_inline)) auto
uninitialized() -> T {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  uint8_t bytes[sizeof(T)];
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  return std::move(*reinterpret_cast<T*>(bytes));
}
