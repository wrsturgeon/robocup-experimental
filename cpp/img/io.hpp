#pragma once

#include "util/ints.hpp"
#include "util/uninitialized.hpp"
#include "util/units.hpp"

#include "stb-image.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

namespace img {

template <u8 C>
class t {
 private:
  u8* data = uninitialized<u8* const>();
  std::array<ChannelMap, C> map = uninitialized<std::array<ChannelMap, C>>();
 public:
  explicit t(char const* fpath) NOX;
  t(t<C> const&) = delete;
  auto operator=(t<C> const&) -> t<C>& = delete;
  t(t<C>&&) = delete;
  auto operator=(t<C>&&) -> t<C>& = delete;
  ~t() noexcept { stbi_image_free(data); }
  pure auto operator[](u8 i) const -> ChannelMap const&;
};

template <u8 C>
t<C>::t(char const* const fpath) NOX {
  int w, h, c;
  data = stbi_load(fpath, &w, &h, &c, C);
#ifndef NDEBUG
  if (!data) { throw std::runtime_error{stbi_failure_reason()}; }
  if ((w != static_cast<int>(kImageW)) || (h != static_cast<int>(kImageH))) {
    throw std::runtime_error{
          "Image size mismatch: supposed to be " +  //
          std::to_string(kImageW) + "x" + std::to_string(kImageH) + " but actually " + std::to_string(w) + "x" +
          std::to_string(h)};
  }
  if (c != static_cast<int>(C)) {
    throw std::runtime_error{
          "Image channel mismatch: supposed to have " + std::to_string(C) + " channels but actually has " + std::to_string(c)};
  }
#endif  // NDEBUG
  for (u8 i = 0; i < C; ++i) {
    new (&map[i]) ChannelMap{data + i};  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  }
}

template <u8 C>
pure auto t<C>::operator[](u8 i) const -> ChannelMap const& {
  assert(i < C);
  return map[i];
}

}  // namespace img
