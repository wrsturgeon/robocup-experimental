#include "measure/units.hpp"
#include "gtest.hpp"

class UnitsPosTTest : public measure::pos_t, public testing::Test {
protected:
  void SetUp() override { internal = 1 << lc; }
};

TEST(Units, PosTZero) { EXPECT_FLOAT_EQ(measure::pos_t{0}.mm(), 0.f); }
TEST(Units, PosTOne) { EXPECT_FLOAT_EQ(measure::pos_t{1}.mm(), 1.f); }
TEST(Units, PosTOverflow) { EXPECT_THROW(measure::pos_t{static_cast<int16_t>(0x7fff)}, std::overflow_error); }
TEST(Units, PosTOverflowNegative) { EXPECT_THROW(measure::pos_t{static_cast<int16_t>(0x8000)}, std::overflow_error); }
TEST(Units, PosTMeters) { EXPECT_FLOAT_EQ(measure::pos_t{1000}.meters(), 1.f); }
TEST(Units, PosTMetersImprecise) { EXPECT_FLOAT_EQ(measure::pos_t{1}.meters(), 0.001f); }
TEST(Units, PosTString) { EXPECT_EQ(measure::pos_t{1000}.operator std::string(), "1.000m"); }
TEST_F(UnitsPosTTest, Int16) { EXPECT_EQ(operator int16_t(), 1); }
