#pragma once

#include "measure/units.hpp"
#include "vision/image-api.hpp"

#include <stdint.h>

namespace training {

template <vision::pxidx_t w, vision::pxidx_t h>
struct Memory {
  measure::Position nao_pos, ball_pos;
  vision::NaoImage<w, h> im;
};

}  // namespace training
