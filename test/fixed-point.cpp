#include "fp/fixed-point.hpp"

#include "gtest.hpp"

#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-result"
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

using fpu = fp::t<16, 8, unsigned>;
using fps = fp::t<16, 8, signed>;

TEST(FixedPoint, ZeroUnsgnd) { ASSERT_FALSE((fpu::zero())); }

TEST(FixedPoint, AddZeroUnsgnd) { ASSERT_FALSE((fpu::zero() + fpu::zero())); }
TEST(FixedPoint, AddZeroSigned) { ASSERT_FALSE((fps::zero() + fps::zero())); }

TEST(FixedPoint, SubZeroUnsgnd) { ASSERT_FALSE((fpu::zero() - fpu::zero())); }
TEST(FixedPoint, SubZeroSigned) { ASSERT_FALSE((fps::zero() - fps::zero())); }

TEST(FixedPoint, FGreaterBUnsgndZero) { ASSERT_FALSE((fp::t<16, 32, unsigned>::zero())); }
TEST(FixedPoint, FGreaterBSignedZero) { ASSERT_FALSE((fp::t<16, 32, signed>::zero())); }

TEST(FixedPoint, FGreaterBUnsgndZeroNeg) { ASSERT_FALSE((-fp::t<16, 32, unsigned>::zero())); }
TEST(FixedPoint, FGreaterBSignedZeroNeg) { ASSERT_FALSE((-fp::t<16, 32, signed>::zero())); }

TEST(FixedPoint, PpPb16f8) { ASSERT_EQ(fps::unit() + fps::unit(), fps::p2(1)); }
TEST(FixedPoint, PpNb16f8) { ASSERT_FALSE(fps::unit() + -fps::unit()); }
TEST(FixedPoint, PmPb16f8) { ASSERT_FALSE(fps::unit() - fps::unit()); }
TEST(FixedPoint, PmNb16f8) { ASSERT_EQ(fps::unit() - -fps::unit(), fps::p2(1)); }
TEST(FixedPoint, NpPb16f8) { ASSERT_FALSE(-fps::unit() + fps::unit()); }
TEST(FixedPoint, NpNb16f8) { ASSERT_EQ(-fps::unit() + -fps::unit(), -fps::p2(1)); }
TEST(FixedPoint, NmPb16f8) { ASSERT_EQ(-fps::unit() - fps::unit(), -fps::p2(1)); }
TEST(FixedPoint, NmNb16f8) { ASSERT_FALSE(-fps::unit() - -fps::unit()); }

using fpunit_t = fp::t<8, 7, unsigned>;
using fpsgnu_t = fp::t<8, 6, signed>;
TEST(FixedPoint, PpPb8f7) { ASSERT_THROW(fpsgnu_t::unit() + fpsgnu_t::unit(), std::out_of_range); }
TEST(FixedPoint, PpNb8f7) { ASSERT_FALSE(fpsgnu_t::unit() + -fpsgnu_t::unit()); }
TEST(FixedPoint, PmPb8f7) { ASSERT_FALSE(fpsgnu_t::unit() - fpsgnu_t::unit()); }
TEST(FixedPoint, PmNb8f7) { ASSERT_THROW(fpsgnu_t::unit() - -fpsgnu_t::unit(), std::out_of_range); }
TEST(FixedPoint, NpPb8f7) { ASSERT_FALSE(-fpsgnu_t::unit() + fpsgnu_t::unit()); }
TEST(FixedPoint, NpNb8f7) { ASSERT_EQ(-fpsgnu_t::unit() + -fpsgnu_t::unit(), (-fpsgnu_t::unit()) << 1); }
TEST(FixedPoint, NmPb8f7) { ASSERT_EQ(-fpsgnu_t::unit() - fpsgnu_t::unit(), (-fpsgnu_t::unit()) << 1); }
TEST(FixedPoint, NmNb8f7) { ASSERT_FALSE(-fpsgnu_t::unit() - -fpsgnu_t::unit()); }

TEST(FixedPoint, AddOverflow) { ASSERT_THROW((fpunit_t::unit() + fpunit_t::unit()), std::out_of_range); }
TEST(FixedPoint, SubOverflow) { ASSERT_THROW((-fpsgnu_t::unit() - (fpsgnu_t::unit() | 1)), std::out_of_range); }

TEST(FixedPoint, MostNegativePass) { ASSERT_EQ((-fps::unit()) << 1, (-fpsgnu_t::unit()) << 1); }
TEST(FixedPoint, NegateMostNegative) { ASSERT_THROW(-((-fpsgnu_t::unit()) << 1), std::out_of_range); }
TEST(FixedPoint, NegateUnsigned) { ASSERT_THROW(-(fpunit_t::unit() | 1), std::out_of_range); }
TEST(FixedPoint, NegateUnsignedOkay) { ASSERT_EQ(-fpunit_t::unit(), -fps::unit()); }  // most negative value

TEST(FixedPoint, Unit8bUnsgnd) {
  auto s = fpunit_t::unit().expose();
  ASSERT_STREQ(s.substr(s.length() - 11).c_str(), " [8b7f:128]");
}

TEST(FixedPoint, Unit8bSigned) {
  auto s = fpsgnu_t::unit().expose();
  ASSERT_STREQ(s.substr(s.length() - 10).c_str(), " [8b6f:64]");
}

TEST(FixedPoint, StupidPrintStatement) { ASSERT_NO_THROW(std::cout << fpsgnu_t::unit() << std::endl); }

TEST(FixedPoint, ZeroEqZeroBigger) { ASSERT_EQ(fpu::zero(), fpunit_t::zero()); }
TEST(FixedPoint, ZeroEqZeroSmallr) { ASSERT_EQ(fpunit_t::zero(), fpu::zero()); }

TEST(FixedPoint, ConvertUtoU) { ASSERT_FALSE(static_cast<fpu>(fpu::zero())); }
TEST(FixedPoint, ConvertUtoS) { ASSERT_FALSE(static_cast<fps>(fpu::zero())); }
TEST(FixedPoint, ConvertStoU) { ASSERT_FALSE(static_cast<fpu>(fps::zero())); }
TEST(FixedPoint, ConvertStoS) { ASSERT_FALSE(static_cast<fps>(fps::zero())); }
TEST(FixedPoint, ConvertNegToUnsigned) { ASSERT_THROW(static_cast<fpu>(-fps::unit()), std::out_of_range); }

TEST(FixedPoint, ZeroArray) { ASSERT_FALSE((fp::a<1, fpu>::zero())); }

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
#pragma clang diagnostic pop
