#include "sdl/context.hpp"

namespace sdl {



Context::~Context() {
  SDL_Quit();
}

Context::Context() {
  if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error{
        std::string{"Couldn't initialize SDL: "} +
        SDL_GetError()};
}



} // namespace sdl

#endif // SDL_CONTEXT_HPP_

#else // SDL_ENABLED
#pragma message("Skipping context.hpp; sdl module disabled")
#endif // SDL_ENABLED
