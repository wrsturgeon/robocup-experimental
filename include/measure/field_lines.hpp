#if MEASURE_ENABLED
#ifndef MEASURE_FIELD_LINES_HPP_
#define MEASURE_FIELD_LINES_HPP_

#include "measure/units.hpp"
#include "rnd/xoshiro.hpp"

namespace measure {



INLINE Position sample_field_lines();



} // namespace measure

#endif // MEASURE_FIELD_LINES_HPP_

#else // MEASURE_ENABLED
#pragma message("Skipping field_lines.hpp; measure module disabled")
#endif // MEASURE_ENABLED
