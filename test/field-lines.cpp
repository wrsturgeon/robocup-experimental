#include "measure/field-lines.hpp"
#include "gtest.hpp"

TEST(FieldLines, sample_65536_times) { uint16_t i = 0; do { ASSERT_NO_THROW(measure::sample_field_lines()); } while (++i); }
