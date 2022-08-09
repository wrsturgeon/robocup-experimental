#pragma once

#include <utility>  // std::move

template <typename T>
__attribute__((always_inline)) inline static auto uninitialized() -> T {
  uint8_t bytes[sizeof(T)];
  return std::move(*reinterpret_cast<T*>(bytes));
}
