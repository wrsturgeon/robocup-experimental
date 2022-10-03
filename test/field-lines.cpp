#include "measure/field-lines.hpp"

#include "gtest.hpp"

TEST(FieldLines, Sample65536Times) {
  std::uint16_t i = 0;
#pragma unroll  // NOLINTNEXTLINE(altera-unroll-loops)
  do {
    ASSERT_NO_THROW(std::cout << measure::sample_field_lines() << std::endl);
  } while (++i);
}
