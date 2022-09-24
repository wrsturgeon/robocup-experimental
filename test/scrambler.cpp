#include "ml/scrambler.hpp"

#include "gtest.hpp"

TEST(Scrambler, ByteAddress) { ASSERT_NO_THROW((ml::Scrambler<uint8_t, 8>{})); }
