#pragma once

#include <cstdint>
#include <utility>  // std::move

template <typename T>
[[nodiscard]] INLINE auto uninitialized() -> T {
  std::byte bytes[sizeof(T)];  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,clang-analyzer-core.uninitialized.UndefReturn)
  return *reinterpret_cast<T*>(bytes);
}
