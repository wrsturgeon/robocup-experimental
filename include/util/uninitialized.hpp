#pragma once

#include <utility>  // std::move

template <typename T>
inline static auto uninitialized() -> T {
  uint8_t bytes[sizeof(T)];
  return std::move(*reinterpret_cast<T*>(bytes));
}
