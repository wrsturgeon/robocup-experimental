#if TRAINING_ENABLED
#ifndef TRAINING_MEMORY_HPP_
#define TRAINING_MEMORY_HPP_

#include <stdint.h>

#include <measure/units.hpp>
#include <rnd/xoshiro.hpp>
#include <vision/image_api.hpp>

namespace training {



template <vision::pxidx_t w, vision::pxidx_t h>
struct Memory {
  measure::Position nao_pos, ball_pos;
  vision::NaoImage<w, h> im;
};



} // namespace training

#endif // TRAINING_MEMORY_HPP_

#else // TRAINING_ENABLED
#pragma message("Skipping memory.hpp; training module disabled")
#endif // TRAINING_ENABLED
