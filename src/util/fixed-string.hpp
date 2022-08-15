#pragma once

// https://vector-of-bool.github.io/2021/10/22/string-templates.html

#include <cstddef>

template <std::size_t N> class FixedString {
 public:
  constexpr FixedString(char const lit[N]) { memcpy(data, lit, N); }
  operator char const*() const { return data; }
 private:
  char data[N + 1];
};
