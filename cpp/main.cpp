#include "ml/optimizer.hpp"
#include "vision/pyramid.hpp"

#ifndef NDEBUG
#include "img/io.hpp"
#include "vision/visualizer.hpp"
#endif  // NDEBUG

#include "util/units.hpp"

#ifndef NDEBUG
#include <filesystem>
#include <iostream>
#include <stdexcept>
#endif  // NDEBUG

auto
main() -> int {
#ifndef NDEBUG
  try {
    auto p = vision::Pyramid<kImageH, kImageW>{"../img/blurred.png"};
    p.save(std::filesystem::current_path() / "_PYRAMID");          // WHAT THE FUCK THIS OPERATOR/ IS SO COOL WHAT
    vision::display_estimate<(1 << 12)>(p, vision::Projection{});  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  {
    auto x = fp::t<fp::kCompactBits, 0, signed>::p2<-1>();
    auto adam = ml::AdamL1<fp::t<fp::kCompactBits, 0, signed>>{};
    auto s = uninitialized<decltype(adam.step(x))>();
    u8 i = 0;
    do {
      s = adam.step(x);
      std::cout << x << " - " << s << " = " << (x -= s) << std::endl;
    } while (++i);
  }
  try {
    static constexpr u8 jump = 16;
    static_assert(256 % jump == 0);  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    u8 i = jump;
    u8 j;
    pxidx_t x;
    pxidx_t y;
    auto p = vision::Pyramid<kImageH, kImageW>{"../img/blurred.png"};
    do {
      j = jump;
      do {
        y = (static_cast<ufull_t>(kImageH) * i) >> 8;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        x = (static_cast<ufull_t>(kImageW) * j) >> 8;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        img::save_and_pinpoint<kImageH, kImageW>(p.array, std::filesystem::current_path() / ("_COORD_" + std::to_string(i) + "_" + std::to_string(j) + ".png"), y, x);
        auto [m, n] = p.find_imprecise({y, x});  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        img::save_and_pinpoint<kImageH, kImageW>(p.array, std::filesystem::current_path() / ("_COORD_" + std::to_string(i) + "_" + std::to_string(j) + "_FOUND.png"), static_cast<pxidx_t>(y + m), static_cast<pxidx_t>(x + n));
      } while (j += jump);  // NOLINT(altera-id-dependent-backward-branch)
    } while (i += jump);    // NOLINT(altera-id-dependent-backward-branch)
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
#endif  // NDEBUG
  return 0;
}
