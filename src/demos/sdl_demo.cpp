#include "sdl/window.hpp"
#include "vision/image_api.hpp"
int main() {
  static constexpr vision::pxidx_t w = 640, h = 480;
  vision::NaoImage<w, h> im;
  // Eventually, load some image here (through Eigen)
  sdl::Window{w, h}.display(im, true);
}