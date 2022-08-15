#pragma once

// https://vector-of-bool.github.io/2021/10/22/string-templates.html

#include <cstddef>

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays)
// NOLINTBEGIN(modernize-avoid-c-arrays)

template <std::size_t N> class FixedString {
 public:
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
  explicit constexpr FixedString(char const lit[N]) { memcpy(data, lit, N); }
  explicit operator char const*() const { return data; }
 private:
  char data[N + 1];
};

// NOLINTEND(modernize-avoid-c-arrays)
// NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays)
