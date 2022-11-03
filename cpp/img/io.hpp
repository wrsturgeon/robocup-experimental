#pragma once

#include "stb-image.hpp"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace img {

// Best possible (but still awkward) solution for STB-Image in initializer lists
inline int STBI_W, STBI_H, STBI_C;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

template <imsize_t H, imsize_t W, u8 C>
void
save(u8 const* const data, std::filesystem::path const& fpath) {
  assert(fpath.extension() == ".png");
  std::filesystem::path p = fpath;
  if (p.is_relative()) { p = std::filesystem::current_path() / p; }
  std::cout << "Saving " << p << "..." << std::endl;
  stbi_write_png(p.c_str(), W, H, C, data, W * C);
  assert(std::filesystem::exists(p));
}

template <imsize_t H, imsize_t W, u8 C, typename T>
requires requires (T const& x) { /* clang-format off */ { x.eval().data() } -> std::convertible_to<u8 const*>; /* clang-format on */
         }
void
save(T const& x, std::filesystem::path const& fpath) {
  save<H, W, C>(x.eval().data(), fpath);
}  // calls the above

template <imsize_t H, imsize_t W, typename T>
requires requires (T const& x, std::filesystem::path const& fpath) { save<H, W, 1>(x, fpath); }
void
save(T const& x, std::filesystem::path const& fpath) {
  save<H, W, 1>(x, fpath);
}  // calls the above

template <imsize_t H, imsize_t W, typename T>
// requires requires (T const& x, Tensor<H, W, 3> const& t) { t.chip(Eigen::fix<2>, Eigen::fix<2>) = x; }
void
save_and_pinpoint(T const& t, std::filesystem::path const& fpath, pxidx_t y, pxidx_t x) {
  // not efficient whatsoever but never even parsed in release mode
  assert(y < H);
  assert(x < W);
  Tensor<H, W, 3> tmp;
  for (pxidx_t i = 0; i < H; ++i) {
    for (pxidx_t j = 0; j < W; ++j) {
      tmp(i, j, Eigen::fix<0>) = t(i, j);
      tmp(i, j, Eigen::fix<1>) = t(i, j);
      tmp(i, j, Eigen::fix<2>) = t(i, j);
    }
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SET_RED(i, j)                                                                                                                                                                                                                                         \
  tmp(i, j, 0) = 255;                                                                                                                                                                                                                                         \
  tmp(i, j, 1) = 0;                                                                                                                                                                                                                                           \
  tmp(i, j, 2) = 0
  SET_RED(y, x);
  if ((x - 2 > 0) and (y - 2 > 0)) { SET_RED(y - 2, x - 2); }
  if ((x - 1 > 0) and (y - 1 > 0)) { SET_RED(y - 1, x - 1); }
  if ((x - 1 > 0) and (y + 1 < H)) { SET_RED(y + 1, x - 1); }
  if ((x - 2 > 0) and (y + 2 < H)) { SET_RED(y + 2, x - 2); }
  if ((x + 2 < W) and (y - 2 > 0)) { SET_RED(y - 2, x + 2); }
  if ((x + 1 < W) and (y - 1 > 0)) { SET_RED(y - 1, x + 1); }
  if ((x + 1 < W) and (y + 1 < H)) { SET_RED(y + 1, x + 1); }
  if ((x + 2 < W) and (y + 2 < H)) { SET_RED(y + 2, x + 2); }
#undef SET_RED
  save<H, W, 3>(tmp.data(), fpath);
}

template <imsize_t H = kImageH, imsize_t W = kImageW, u8 C = 3>
class t {
 private:
  u8* data;
  using channel_map_t = Eigen::Map<Array<H, W, u8>, RowMajor<H, W>, Eigen::InnerStride<3>>;
 public:
  explicit t(std::filesystem::path const& fpath);
  t(t<H, W, C> const&) = delete;
  auto operator=(t<H, W, C> const&) -> t<H, W, C>& = delete;
  t(t<H, W, C>&&) = delete;
  auto operator=(t<H, W, C>&&) -> t<H, W, C>& = delete;
  ~t() noexcept { stbi_image_free(data); }
  template <u8 i>
  requires (i < C)  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  pure auto channel() const -> channel_map_t /* clang-format off */ { return channel_map_t{data + i}; }                 /* clang-format on */
  pure auto collapse() const -> decltype(auto) requires (C == 3);
};

template <imsize_t H, imsize_t W, u8 C>
t<H, W, C>::t(std::filesystem::path const& fpath) : data{stbi_load(fpath.c_str(), &STBI_W, &STBI_H, &STBI_C, C)} {
  if (data == nullptr) { throw std::runtime_error{stbi_failure_reason()}; }
  if ((STBI_W != static_cast<int>(W)) or (STBI_H != static_cast<int>(H))) {
    throw std::runtime_error{
          "Image size mismatch: supposed to be " +                          //
          std::to_string(W) + "x" + std::to_string(H) + " but actually " +  //
          std::to_string(STBI_W) + "x" + std::to_string(STBI_H)};
  }
  if (STBI_C != static_cast<int>(C)) { throw std::runtime_error{"Image channel mismatch: supposed to have " + std::to_string(C) + " channels but actually has " + std::to_string(STBI_C)}; }
}

}  // namespace img
