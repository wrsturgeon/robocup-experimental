#if TRAINING_ENABLED
#ifndef TRAINING_MEMORY_HPP_
#define TRAINING_MEMORY_HPP_

#include <vision/image_api.hpp>

#include <stdint.h>

#include <measure/units.hpp>
#include <rnd/xoshiro.hpp>

namespace training {



template <vision::pxidx_t w, vision::pxidx_t h>
struct Memory {
  measure::Position nao_pos, ball_pos;
  vision::NaoImage<w, h> img;
};



} // namespace training

#endif // TRAINING_MEMORY_HPP_

#else // TRAINING_ENABLED
#pragma message("Skipping memory.hpp; training module disabled")
#endif // TRAINING_ENABLED
