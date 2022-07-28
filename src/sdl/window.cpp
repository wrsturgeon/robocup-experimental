#include "sdl/window.hpp"

namespace sdl {



class Window::Closed : public std::runtime_error { using std::runtime_error::runtime_error; };



Window::Window(int w, int h, char const *const title) {
  window = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
#if __APPLE__
        w >> 1, h >> 1,
#else
        w, h,
#endif
        SDL_WINDOW_ALLOW_HIGHDPI);
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
}


MEMBER_INLINE void Window::display(SDL_Surface* surface, bool popup = false);

MEMBER_INLINE float Window::brightness() { return SDL_GetWindowBrightness(window); }
MEMBER_INLINE void const* Window::data(char const *const name) { return SDL_GetWindowData(window, name); }
MEMBER_INLINE uint32_t Window::flags() { return SDL_GetWindowFlags(window); }
MEMBER_INLINE bool Window::input_grabbed() { return SDL_GetWindowGrab(window); }
MEMBER_INLINE Coordinate Window::max_size() { Coordinate c; SDL_GetWindowMaximumSize(window, &c.x, &c.y); return c; }
MEMBER_INLINE Coordinate Window::min_size() { Coordinate c; SDL_GetWindowMinimumSize(window, &c.x, &c.y); return c; }
MEMBER_INLINE Coordinate Window::position() { Coordinate c; SDL_GetWindowPosition(window, &c.x, &c.y); return c; }
MEMBER_INLINE Coordinate Window::size() { Coordinate c; SDL_GetWindowSize(window, &c.x, &c.y); return c; }
MEMBER_INLINE char const* Window::title() { return SDL_GetWindowTitle(window); }



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Custom member functions



MEMBER_INLINE void Window::handle_events() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        throw Closed{"Window manually closed"};
      default:
        SDL_UpdateWindowSurface(window);
    }
  }
}



MEMBER_INLINE void Window::display(SDL_Surface* surface, bool popup) {
  SDL_BlitSurface(surface, nullptr, surface, nullptr);

  if (popup) {
    try {
      do {
        handle_events();
      } while (true);
    } catch (Closed) {}
  }
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



} // namespace sdl
