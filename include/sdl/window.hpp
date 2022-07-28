#if SDL_ENABLED
#ifndef SDL_WINDOW_HPP_
#define SDL_WINDOW_HPP_

#include <stdexcept>
#include <string>

#include "SDL.h"
/**
 *  Problem at SDL_stdinc.h:35:
 *    #define _DARWIN_C_SOURCE 1 // for memset_pattern4()
 *  memset_pattern4 defined at /Library/Developer/CommandLineTools/SDKs/MacOSX12.0.sdk/usr/include/string.h:156
 *    ...but only #if __DARWIN_C_LEVEL >= __DARWIN_C_FULL (which is 900000)
 *    __DARWIN_C_LEVEL is defined 3 different ways, all in sys/cdefs.h lines 737-741:
 *      Verbatim:
 *        #if   defined(_ANSI_SOURCE)
 *        #define __DARWIN_C_LEVEL        __DARWIN_C_ANSI
 *        #elif defined(_POSIX_C_SOURCE) && !defined(_DARWIN_C_SOURCE) && !defined(_NONSTD_SOURCE)
 *        #define __DARWIN_C_LEVEL        _POSIX_C_SOURCE
 *        #else
 *        #define __DARWIN_C_LEVEL        __DARWIN_C_FULL
 *        #endif
 *      In this project, we CAN'T define _ANSI_SOURCE or _POSIX_C_SOURCE if we want SDL2
 *        Which means we can't define _XOPEN_SOURCE (as we were before), since that defines _POSIX_C_SOURCE
 *        Well, shit--workaround in progress
 */

#include "sdl/context.hpp"

namespace sdl {



struct BorderSizes { int left, right, top, bottom; };

struct Coordinate { int x, y; };

struct ColorData { uint16_t r, g, b; };



class Window {
public:
  Window(Window const&) = delete;
  Window(int w, int h, char const *const title = "UPennalizers");
  ~Window();
  MEMBER_INLINE void display(SDL_Surface* surface, bool popup = false);
protected:

  // Member variables
  SDL_Window* window;
  SDL_Surface* surface;

  // Custom member functions
  MEMBER_INLINE void handle_events();
  class Closed;

  // Ported from C-style SDL
  MEMBER_INLINE BorderSizes border_sizes();
  MEMBER_INLINE int display_index();
  MEMBER_INLINE SDL_DisplayMode display_mode();
  MEMBER_INLINE ColorData gamma_ramp();
  MEMBER_INLINE uint32_t id();
  MEMBER_INLINE float opacity();
  MEMBER_INLINE uint32_t pixel_format();
  MEMBER_INLINE float brightness();
  MEMBER_INLINE void const* data(char const *const name);
  MEMBER_INLINE uint32_t flags();
  MEMBER_INLINE bool input_grabbed();
  MEMBER_INLINE Coordinate max_size();
  MEMBER_INLINE Coordinate min_size();
  MEMBER_INLINE Coordinate position();
  MEMBER_INLINE Coordinate size();
  MEMBER_INLINE char const* title();
};



} // namespace sdl

#endif // SDL_WINDOW_HPP_

#else // SDL_ENABLED
#pragma message("Skipping window.hpp; sdl module disabled")
#endif // SDL_ENABLED
