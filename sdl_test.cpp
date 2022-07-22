#include <sdl/window.hpp>
#include <vision/image_api.hpp>
int main() {
  vision::NaoImage<320, 240> img;
  sdl::Window bs(img);
}