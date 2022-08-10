#include "measure/units.hpp"

#include "gtest.hpp"

TEST(Units, PosTZero) { ASSERT_FLOAT_EQ(measure::pos_t{0}.mm(), 0.f); }

TEST(Units, PosTOne) { ASSERT_FLOAT_EQ(measure::pos_t{1}.mm(), 1.f); }

TEST(Units, PosTAnswer) { ASSERT_FLOAT_EQ(measure::pos_t{42}.mm(), 42.f); }

TEST(Units, PosTOverflow) {
  ASSERT_THROW(
        measure::pos_t{static_cast<int16_t>(0x7fff)}, std::overflow_error);
}

TEST(Units, PosTOverflowNegative) {
  ASSERT_THROW(
        measure::pos_t{static_cast<int16_t>(0x8000)}, std::overflow_error);
}

TEST(Units, PosTMeters) { ASSERT_FLOAT_EQ(measure::pos_t{1000}.meters(), 1.f); }

TEST(Units, PosTMetersImprecise) {
  ASSERT_FLOAT_EQ(measure::pos_t{1}.meters(), 0.001f);
}

TEST(Units, PosTString) {
  ASSERT_EQ(static_cast<std::string>(measure::pos_t{1000}), "1.000000m");
}

TEST(Units, PosTStream) {
  std::ostream os{nullptr};
  ASSERT_NO_THROW(os << measure::pos_t{42});
}

TEST(Units, PositionString) {
  ASSERT_EQ(static_cast<std::string>(measure::Position{1000, 1000}), "(1.000000m x, 1.000000m y)");
}

TEST(Units, PositionStream) {
  std::ostream os{nullptr};
  ASSERT_NO_THROW(os << (measure::Position{42, 42}));
}
