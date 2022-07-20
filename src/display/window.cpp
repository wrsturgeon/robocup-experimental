#ifndef DISPLAY_ON_WINDOW_HPP_
#define DISPLAY_ON_WINDOW_HPP_
#include <util/options.hpp>
#if DISPLAY_ON

#include <SDL.h>

#include <iostream>



int main() {

  if (SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "Couldn't initialize SDL: " << SDL_GetError() << '\n';
    return 1;
  }

  // Twice resolution on Apple Retina DISPLAY_ONs
  #if __APPLE__
  static constexpr int WindowW = IMAGE_W >> 1;
  static constexpr int WindowH = IMAGE_H >> 1;
  #else
  static constexpr int WindowW = IMAGE_W;
  static constexpr int WindowH = IMAGE_H;
  #endif

  SDL_Window *const window = SDL_CreateWindow(
        "SDL Test Window", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, WindowW, WindowH,
        0);
  if (!window) {
    std::cerr << "Couldn't create SDL window: " << SDL_GetError() << '\n';
    return 1;
  }

  SDL_Surface *const window_surface = SDL_GetWindowSurface(window);
  if (!window_surface) {
    std::cerr << "Failed to get the surface from the window\n";
    return 1;
  }

  SDL_Event event;
  do {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          goto CLEANUP;
        default:
          SDL_UpdateWindowSurface(window);
      }
    }
  } while (true);

CLEANUP:
  SDL_DestroyWindow(window);
}



#endif // DISPLAY_ON

#endif // DISPLAY_ON_WINDOW_HPP_
