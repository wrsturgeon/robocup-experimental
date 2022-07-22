#include <options.hpp>
#if SDL_ENABLED
#ifndef SDL_WINDOW_HPP_
#define SDL_WINDOW_HPP_

#include <SDL.h>

#include <stdexcept>
#include <string>

#include <util/specifiers.hpp>
#include <vision/image_api.hpp>

namespace sdl {



class WindowClosedError : public std::runtime_error { using std::runtime_error::runtime_error; };

struct BorderSizes {
  int left, right, top, bottom;
};

struct Coordinate {
  int x, y; // SDL uses raw ints
};

struct ColorData {
  uint16_t r, g, b;
};



class Window {
public:
  Window(Window const&) = delete;
  Window(int w, int h, char const *const title);
  ~Window();
protected:

  // Member variables
  bool const owns_sdl_init;
  SDL_Window* window;
  SDL_Surface* surface;

  // Custom member functions
  MEMBER_INLINE void handle_events();
  MEMBER_INLINE void display();

  // Ported from C-style SDL
  MEMBER_INLINE BorderSizes border_sizes();
  MEMBER_INLINE int display_index();
  MEMBER_INLINE SDL_DisplayMode display_mode();
  MEMBER_INLINE ColorData gamma_ramp();
  MEMBER_INLINE uint32_t id();
  MEMBER_INLINE float opacity();
  MEMBER_INLINE uint32_t pixel_format();
  MEMBER_INLINE float brightness() { return SDL_GetWindowBrightness(window); }
  MEMBER_INLINE void const* data(char const *const name) { return SDL_GetWindowData(window, name); }
  MEMBER_INLINE uint32_t flags() { return SDL_GetWindowFlags(window); }
  MEMBER_INLINE bool input_grabbed() { return SDL_GetWindowGrab(window); }
  MEMBER_INLINE Coordinate max_size() { Coordinate c; SDL_GetWindowMaximumSize(window, &c.x, &c.y); return c; }
  MEMBER_INLINE Coordinate min_size() { Coordinate c; SDL_GetWindowMinimumSize(window, &c.x, &c.y); return c; }
  MEMBER_INLINE Coordinate position() { Coordinate c; SDL_GetWindowPosition(window, &c.x, &c.y); return c; }
  MEMBER_INLINE Coordinate size() { Coordinate c; SDL_GetWindowSize(window, &c.x, &c.y); return c; }
  MEMBER_INLINE char const* title() { return SDL_GetWindowTitle(window); }
};



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Custom member functions

Window::Window(int w, int h, char const *const title = "UPennalizers")
      : owns_sdl_init{!SDL_WasInit(SDL_INIT_VIDEO)} {
  if (owns_sdl_init) {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO)) throw std::runtime_error{
          std::string{"Couldn't initialize SDL: "} +
          SDL_GetError()};
  }

  window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
#if __APPLE__
        w >> 1, h >> 1,
#else
        w, h,
#endif
        0);
  if (!window) throw std::runtime_error{
        std::string{"Couldn't create SDL window: "} +
        SDL_GetError()};

  surface = SDL_GetWindowSurface(window);
  if (!surface) throw std::runtime_error{
        std::string{"Couldn't get SDL window surface: "} +
        SDL_GetError()};
  
  handle_events();
}



Window::~Window() {
  SDL_DestroyWindow(window);
  if (owns_sdl_init) {
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  }
}



MEMBER_INLINE void Window::handle_events() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        throw std::runtime_error{"Window manually closed"};
      default:
        SDL_UpdateWindowSurface(window);
    }
  }
}



MEMBER_INLINE void Window::display(vision::NaoImage const& img) {
  SDL_BlitSurface(img, nullptr, surface, nullptr);
}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Ported from C-style SDL

MEMBER_INLINE BorderSizes Window::border_sizes() {
  BorderSizes s;
  if (SDL_GetWindowBordersSize(window, &s.top, &s.left, &s.bottom, &s.right)) {
    throw std::runtime_error{
          std::string{"Couldn't get window border sizes: "} +
          SDL_GetError()};
  }
  return s;
}

MEMBER_INLINE int Window::display_index() {
  int r = SDL_GetWindowDisplayIndex(window);
  if (r < 0) throw std::runtime_error{
        std::string{"Couldn't get window display index: "} +
        SDL_GetError()};
  return r;
}

MEMBER_INLINE SDL_DisplayMode Window::display_mode() {
  SDL_DisplayMode m;
  if (SDL_GetWindowDisplayMode(window, &m)) throw std::runtime_error{
        std::string{"Couldn't get window display mode: "} +
        SDL_GetError()};
  return m;
}

MEMBER_INLINE ColorData Window::gamma_ramp() {
  ColorData c;
  if (SDL_GetWindowGammaRamp(window, &c.r, &c.g, &c.b)) throw std::runtime_error{
        std::string{"Couldn't get window gamma ramp: "} +
        SDL_GetError()};
  return c;
}

MEMBER_INLINE uint32_t Window::id() {
  uint32_t r = SDL_GetWindowID(window);
  if (!r) throw std::runtime_error{
        std::string{"Couldn't get window ID: "} +
        SDL_GetError()};
  return r;
}

MEMBER_INLINE float Window::opacity() {
  float r;
  if (SDL_GetWindowOpacity(window, &r)) throw std::runtime_error{
        std::string{"Couldn't get window opacity: "} +
        SDL_GetError()};
  return r;
}

MEMBER_INLINE uint32_t Window::pixel_format() {
  uint32_t r = SDL_GetWindowPixelFormat(window);
  if (r == SDL_PIXELFORMAT_UNKNOWN) throw std::runtime_error{
        std::string{"Couldn't get window pixel format: "} +
        SDL_GetError()};
  return r;
}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Specialized subclasses

class Popup : public Window {
public:
  Popup(int w, int h, char const *const title = "UPennalizers")
        : Window{w, h, title} {
    try {
      do {
        handle_events();
      } while (true);
    } catch (WindowClosedError) {}
  }
};



} // namespace sdl

#endif // SDL_WINDOW_HPP_

#endif // SDL_ENABLED
