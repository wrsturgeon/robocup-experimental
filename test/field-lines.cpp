#include "measure/field-lines.hpp"

#include "gtest.hpp"

TEST(FieldLines, Sample65536Times) {
  std::uint16_t i = 0;
  do { ASSERT_NO_THROW(measure::sample_field_lines()); } while (++i);
}
