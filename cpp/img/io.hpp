#pragma once

#include "img/types.hpp"
#include "util/ints.hpp"
#include "util/units.hpp"

#include "stb-image.hpp"

#include <array>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace img {

// Best possible (but still awkward) solution for STB-Image in initializer lists
inline int STBI_W, STBI_H, STBI_C;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

template <imsize_t H, imsize_t W, u8 C> void
save(u8 const* const data, std::filesystem::path const& fpath) {
  assert(fpath.extension() == ".png");
  std::filesystem::path p = fpath;
  if (p.is_relative()) { p = std::filesystem::current_path() / p; }
  std::cout << "Saving " << p << "..." << std::endl;
  stbi_write_png(p.c_str(), W, H, C, data, W * C);
  assert(std::filesystem::exists(p));
}

template <imsize_t H, imsize_t W, u8 C, TensorExpressible<H, W, C> T> void
save(T x, std::filesystem::path const& fpath) {
  save<H, W, C>(x.eval().data(), fpath);
}  // calls the above

template <imsize_t H, imsize_t W, ArrayExpressible<H, W> T> void
save(T x, std::filesystem::path const& fpath) {
  save<H, W, 1>(x, fpath);
}  // calls the above

template <imsize_t H = kImageH, imsize_t W = kImageW, u8 C = 3> class t {
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
  template <u8 i> requires (i < C)  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  pure auto channel() const -> channel_map_t /* clang-format off */ { return channel_map_t{data + i}; }                                 /* clang-format on */
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
  if (STBI_C != static_cast<int>(C)) {
    throw std::runtime_error{
          "Image channel mismatch: supposed to have " + std::to_string(C) + " channels but actually has " +
          std::to_string(STBI_C)};
  }
}

}  // namespace img
