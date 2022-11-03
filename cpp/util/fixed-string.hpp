#pragma once

#include <array>
#include <compare>

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)

template <ufull_t N1, ufull_t N2>
pure auto
cstrcmp(std::array<char, N1> const& a, std::array<char, N2> const& b) noexcept -> std::strong_ordering {
  if constexpr (N1 != N2) { return N1 <=> N2; }
  for (ufull_t i = 0; i < N1; ++i) {
    if (a[i] != b[i]) { return a[i] <=> b[i]; }
  }
  return std::strong_ordering::equal;
}

template <ufull_t N1, ufull_t N2>
pure auto
cstrcmp(std::array<char, N1> const& a, char const (&b)[N2]) noexcept -> std::strong_ordering {
  if constexpr (N1 != N2) { return N1 <=> N2; }
  for (ufull_t i = 0; i < N1; ++i) {
    if (a[i] != b[i]) { return a[i] <=> b[i]; }
  }
  return std::strong_ordering::equal;
}

template <ufull_t N>
pure auto
cstr_to_array(char const (&str)[N]) noexcept -> std::array<char, N> {
  std::array<char, N> result;  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
  for (ufull_t i = 0; i < N; ++i) { result[i] = str[i]; }
  return result;
}

// stupidest fucking thing but it works lol
template <ufull_t N>
struct FixedString {  // NOLINT(altera-struct-pack-align)
  using arr_t = std::array<char, N>;
  arr_t const a;  // NOLINT(misc-non-private-member-variables-in-classes)
  // NOLINTBEGIN(google-explicit-constructor,hicpp-explicit-conversions)
  constexpr FixedString(char const (&str)[N]) : a{cstr_to_array(str)} {}
  // NOLINTEND(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator arr_t() const noexcept { return a; }  // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
};

template <ufull_t N1, ufull_t N2>
pure auto
operator<=>(FixedString<N1> const& a, FixedString<N2> const& b) -> std::strong_ordering {
  return cstrcmp(a.a, b.a);
}

template <ufull_t N1, ufull_t N2>
pure auto
operator==(FixedString<N1> const& a, FixedString<N2> const& b) -> bool {
  return (a <=> b) == std::strong_ordering::equal;
}

template <ufull_t N1, ufull_t N2>
pure auto
operator<=>(FixedString<N1> const& a, char const (&b)[N2]) -> std::strong_ordering {
  return cstrcmp(a.a, b);
}

template <ufull_t N1, ufull_t N2>
pure auto
operator==(FixedString<N1> const& a, char const (&b)[N2]) -> bool {
  return (a <=> b) == std::strong_ordering::equal;
}

// Deduction guide
template <ufull_t N>
FixedString(char const (&)[N]) -> FixedString<N>;

// NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
