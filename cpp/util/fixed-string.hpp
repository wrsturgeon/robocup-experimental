#pragma once

#include <array>
#include <compare>
#include <cstddef>

template <std::size_t N1, std::size_t N2> pure auto
cstrcmp(std::array<char, N1> const& a, std::array<char, N2> const& b) noexcept -> std::strong_ordering {
  if constexpr (N1 != N2) { return N1 <=> N2; }
  for (std::size_t i = 0; i < N1; ++i) {
    if (a[i] != b[i]) { return a[i] <=> b[i]; }
  }
  return std::strong_ordering::equal;
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
  arr_t const a;  // NOLINT(misc-non-private-member-variables-in-classes)
  // NOLINTBEGIN(google-explicit-constructor,hicpp-explicit-conversions)
  // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  constexpr FixedString(char const (&str)[N]) : a{cstr_to_array(str)} {}
  // NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  // NOLINTEND(google-explicit-constructor,hicpp-explicit-conversions)
  pure operator arr_t() const noexcept { return a; }  // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  template <std::size_t N2> pure auto operator<=>(FixedString<N2> const& s) -> std::strong_ordering;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // this must be an error/eager overapplication of the rule
  // NOLINTNEXTLINE(hicpp-use-nullptr,modernize-use-nullptr)
  template <std::size_t N2> pure auto operator==(FixedString<N2> const& s) -> bool { return (*this <=> s) == 0; }
  // NOLINTNEXTLINE(hicpp-use-nullptr,modernize-use-nullptr)
  template <std::size_t N2> pure auto operator!=(FixedString<N2> const& s) -> bool { return (*this <=> s) != 0; }
#pragma clang diagnostic pop
};

template <std::size_t N> template <std::size_t N2> pure auto
FixedString<N>::operator<=>(FixedString<N2> const& s) -> std::strong_ordering {
  return cstrcmp(a, s.a);
}

// Deduction guide
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
template <std::size_t N> FixedString(char const (&)[N]) -> FixedString<N>;
