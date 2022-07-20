#ifndef DISPLAY_SDL_HPP_
#define DISPLAY_SDL_HPP_
#include <util/options.hpp>
#if DISPLAY_ON

#include <SDL.h>

#include <stdexcept>
#include <string>

namespace sdl {



struct BorderSizes {
  int left, right, top, bottom;
};

struct Coordinate {
  int x, y; // SDL uses raw ints
};

struct ColorData {
  uint16_t r, g, b;
};

class ICCProfile {
public:
  ICCProfile(ICCProfile const&) = delete;
  ICCProfile() = delete;
  ~ICCProfile() { SDL_free(data); }
  operator void const *const() const { return data; }
protected:
  friend class Window;
  ICCProfile(SDL_Window *const w, size_t *const s);
  void *const data;
};

ICCProfile::ICCProfile(SDL_Window *const w, size_t *const s) : data{SDL_GetWindowICCProfile(w, s)} {
  if (!data) throw std::runtime_error{std::string{"Failed to get ICC profile: "} + SDL_GetError()};
}



class Window {
public:
  Window(Window const&) = delete;
  Window(int w, int h, char const *const title);
  ~Window();
protected:
  bool const owns_sdl_init;
  SDL_Window* window;
  SDL_Surface* surface;
  void handle_events();
  BorderSizes border_sizes();
  int display_index();
  SDL_DisplayMode display_mode();
  ColorData gamma_ramp();
  uint32_t id();
  float opacity();
  uint32_t pixel_format();
  float brightness() { return SDL_GetWindowBrightness(window); }
  void const* data(char const *const name) { return SDL_GetWindowData(window, name); }
  uint32_t flags() { return SDL_GetWindowFlags(window); }
  bool input_grabbed() { return SDL_GetWindowGrab(window); }
  ICCProfile icc_profile(size_t s) { return ICCProfile(window, &s); }
  bool keyboard_grabbed() { return SDL_GetWindowKeyboardGrab(window); }
  Coordinate max_size() { Coordinate c; SDL_GetWindowMaximumSize(window, &c.x, &c.y); return c; }
  Coordinate min_size() { Coordinate c; SDL_GetWindowMinimumSize(window, &c.x, &c.y); return c; }
  bool mouse_grabbed() { return SDL_GetWindowMouseGrab(window); }
  SDL_Rect const* mouse_rect() { return SDL_GetWindowMouseRect(window); }
  Coordinate position() { Coordinate c; SDL_GetWindowPosition(window, &c.x, &c.y); return c; }
  Coordinate size() { Coordinate c; SDL_GetWindowSize(window, &c.x, &c.y); return c; }
  char const* title() { return SDL_GetWindowTitle(window); }
};



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



void Window::handle_events() {
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



BorderSizes Window::border_sizes() {
  BorderSizes s;
  if (SDL_GetWindowBordersSize(window, &s.top, &s.left, &s.bottom, &s.right)) {
    throw std::runtime_error{
          std::string{"Couldn't get window border sizes: "} +
          SDL_GetError()};
  }
  return s;
}

int Window::display_index() {
  int r = SDL_GetWindowDisplayIndex(window);
  if (r < 0) throw std::runtime_error{
        std::string{"Couldn't get window display index: "} +
        SDL_GetError()};
  return r;
}

SDL_DisplayMode Window::display_mode() {
  SDL_DisplayMode m;
  if (SDL_GetWindowDisplayMode(window, &m)) throw std::runtime_error{
        std::string{"Couldn't get window display mode: "} +
        SDL_GetError()};
  return m;
}

ColorData Window::gamma_ramp() {
  ColorData c;
  if (SDL_GetWindowGammaRamp(window, &c.r, &c.g, &c.b)) throw std::runtime_error{
        std::string{"Couldn't get window gamma ramp: "} +
        SDL_GetError()};
  return c;
}

uint32_t Window::id() {
  uint32_t r = SDL_GetWindowID(window);
  if (!r) throw std::runtime_error{
        std::string{"Couldn't get window ID: "} +
        SDL_GetError()};
  return r;
}

float Window::opacity() {
  float r;
  if (SDL_GetWindowOpacity(window, &r)) throw std::runtime_error{
        std::string{"Couldn't get window opacity: "} +
        SDL_GetError()};
  return r;
}

uint32_t Window::pixel_format() {
  uint32_t r = SDL_GetWindowPixelFormat(window);
  if (r == SDL_PIXELFORMAT_UNKNOWN) throw std::runtime_error{
        std::string{"Couldn't get window pixel format: "} +
        SDL_GetError()};
  return r;
}



} // namespace sdl

#endif // DISPLAY_ON

#endif // DISPLAY_SDL_HPP_
