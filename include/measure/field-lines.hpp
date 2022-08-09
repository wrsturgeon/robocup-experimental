#pragma once

#include "rnd/xoshiro.hpp"
#include "vision/pxpos.hpp"

#include <cstdint>

namespace measure {

auto sample_field_lines() -> Position;

}  // namespace measure
