#if MEASURE_ENABLED
#ifndef MEASURE_GAME_DATA_HPP_
#define MEASURE_GAME_DATA_HPP_

#include <measure/units.hpp>

namespace measure {



struct GameData {
  pos_t nao_pos;
  pos_t ball_pos;
};



} // namespace measure

#endif // MEASURE_GAME_DATA_HPP_

#else // MEASURE_ENABLED
#pragma message("Skipping game_data.hpp; measure module disabled")
#endif // MEASURE_ENABLED
