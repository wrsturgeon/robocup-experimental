#include "measure/units.hpp"
#include "gtest.hpp"

TEST(Units, pos_t_zero) { EXPECT_FLOAT_EQ(measure::pos_t{0}.mm(), 0.f); }
TEST(Units, pos_t_one) { EXPECT_FLOAT_EQ(measure::pos_t{1}.mm(), 1.f); }
TEST(Units, pos_t_overflow) { EXPECT_THROW(measure::pos_t{static_cast<int16_t>(0x7fff)}, std::overflow_error); }
TEST(Units, pos_t_overflow_negative) { EXPECT_THROW(measure::pos_t{static_cast<int16_t>(0x8000)}, std::overflow_error); }
