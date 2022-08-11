#pragma once

// https://vector-of-bool.github.io/2021/10/22/string-templates.html

template <size_t N> class FixedString {
 public:
  constexpr FixedString::FixedString(char const lit[N]) { memcpy(data_, lit, N); }
  operator char const*() const { return data_; }
 private:
  char data[N + 1];
};
