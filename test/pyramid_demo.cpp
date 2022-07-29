#include "wasserstein/pyramid.hpp"

int main() {
  static constexpr vision::pxidx_t w = 640, h = 480;
  uint8_t bs[h][w];
  // Eventually, load some image here (through Eigen)
  wasserstein::Pyramid<w, h>{bs};
}
