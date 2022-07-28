#if SDL_ENABLED
#ifndef SDL_CONTEXT_HPP_
#define SDL_CONTEXT_HPP_

#include <stdexcept>
#include <string>

#include "SDL.h"

namespace sdl {



struct Context {
  Context();
  ~Context();
};



// Global class--I know, but that's how SDL works and has to work
// Can only be instantiated once, thanks to include guards
static Context _GLOBAL_CONTEXT;



} // namespace sdl

#endif // SDL_CONTEXT_HPP_

#else // SDL_ENABLED
#pragma message("Skipping context.hpp; sdl module disabled")
#endif // SDL_ENABLED
