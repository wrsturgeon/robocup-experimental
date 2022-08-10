#include "training/scrambler.hpp"

#include "gtest.hpp"

TEST(Scrambler, ByteAddress) { ASSERT_NO_THROW((training::Scrambler<uint8_t, 8>{})); }
