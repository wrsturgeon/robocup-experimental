#pragma once

#include <array>
#include <compare>
#include <cstddef>

pure auto
cstrcmp(char const* a, char const* b) noexcept -> std::strong_ordering {
  if (!a) { return b ? std::strong_ordering::less : std::strong_ordering::equal; }
  if (!b) { return std::strong_ordering::greater; }
  do {
    if ((*a != *b) || !*a) { return *a <=> *b; }
  } while (++a and ++b);  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  return a <=> b;
}

template <std::size_t N> pure auto
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
cstr_to_array(char const (&str)[N]) noexcept -> std::array<char, N> {
  std::array<char, N> result;  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
  for (std::size_t i = 0; i < N; ++i) { result[i] = str[i]; }
  return result;
}

// stupidest fucking thing but it works lol
template <std::size_t N> struct FixedString {  // NOLINT(altera-struct-pack-align)
  using arr_t = std::array<char, N>;
  using c_arr_t = char const (&)[N];  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  arr_t const a;                      // NOLINT(misc-non-private-member-variables-in-classes)
  // NOLINTBEGIN(google-explicit-constructor,hicpp-explicit-conversions)
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  constexpr FixedString(char const (&str)[N]) : a{cstr_to_array(str)} {}
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  // NOLINTEND(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator arr_t() const noexcept { return a; }  // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator c_arr_t() const noexcept {            // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
    return a.data();
  }  // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  template <std::size_t N2> pure auto operator<=>(FixedString<N2> const& s) -> std::strong_ordering;
};

template <std::size_t N> template <std::size_t N2> pure auto
FixedString<N>::operator<=>(FixedString<N2> const& s) -> std::strong_ordering {
  return cstrcmp(a, s.a);
}

// Deduction guide
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
template <std::size_t N> FixedString(char const (&)[N]) -> FixedString<N>;
