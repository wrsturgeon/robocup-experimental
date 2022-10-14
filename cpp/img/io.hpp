#include "util/ints.hpp"
#include "util/uninitialized.hpp"
#include "util/units.hpp"

#include "stb-image.hpp"

#include <cassert>
#include <stdexcept>

namespace img {

template <int C>
class t {
 private:
  u8* data = uninitialized<u8* const>();
  ImageMap<C> map = uninitialized<ImageMap<C>>();
 public:
  INLINE explicit t(char const* fpath) NOX;
  t(t<C> const&) = delete;
  auto operator=(t<C> const&) -> t<C>& = delete;
  t(t<C>&&) = delete;
  auto operator=(t<C>&&) -> t<C>& = delete;
  ~t() noexcept { stbi_image_free(data); }
};

template <int C>
INLINE t<C>::t(char const* const fpath) NOX {
  int w, h, c;
  data = stbi_load(fpath, &w, &h, &c, C);
#ifndef NDEBUG
  if (!data) {
    throw std::runtime_error{stbi_failure_reason()};
  }
  assert(c == C);
  assert(w == kImageW);
  assert(h == kImageH);
#endif  // NDEBUG
  map = ImageMap<C>{data};
}

}  // namespace img
