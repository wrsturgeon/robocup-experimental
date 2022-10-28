#pragma once

#include <cstddef>
#include <type_traits>

template <typename T> pure auto
uninitialized() -> std::decay_t<T> {
  std::byte bytes[sizeof(T)];  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,clang-analyzer-core.uninitialized.UndefReturn)
  return *reinterpret_cast<T*>(bytes);
}
