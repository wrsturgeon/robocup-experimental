#include "ml/optimizer.hpp"
#include "vision/pyramid.hpp"

#ifndef NDEBUG
#include "img/io.hpp"
#include "vision/visualizer.hpp"
#endif  // NDEBUG

#include "util/ints.hpp"
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
    p.save(std::filesystem::current_path() / "_PYRAMID");  // WHAT THE FUCK THIS OPERATOR/ IS SO COOL WHAT
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    vision::display_estimate<(1 << 12)>(p, vision::Projection{});
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  auto x = fp::t<fp::kCompactBits, 0, signed>::p2<-1>();
  auto adam = ml::AdamL1<fp::t<fp::kCompactBits, 0, signed>>{};
  auto s = uninitialized<decltype(adam.step(x))>();
  u8 i = 0;
  do {
    s = adam.step(x);
    std::cout << x << " - " << s << " = " << (x -= s) << std::endl;
  } while (++i);
#endif  // NDEBUG
  return 0;
}
