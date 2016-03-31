#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "uniform_random.h"


typedef ::testing::Types<
  rnd::UniformRandomSingleThreaded
  , rnd::UniformRandomSpinLock
  , rnd::UniformRandomLockFree
  , rnd::UniformRandomForkable<rnd::FORKED_VERSION_STEP>
> UniformRandomTypeList;


template <typename UniformRandom>
struct UniformRandomTest
      : public ::testing::Test
{
public:
  UniformRandomTest()
    : generator(start)
  {}

  static rnd::ValueType const start = 1;
  UniformRandom generator;
};


TYPED_TEST_CASE(UniformRandomTest, UniformRandomTypeList);


TYPED_TEST(UniformRandomTest, CheckConsecutiveCalls)
{
  EXPECT_NEAR(this->generator(), 0.154508, 0.000001);
  EXPECT_NEAR(this->generator(), 0.981739, 0.000001);
}


// TODO: I cannot check async calls in MSVC's Clang because of compilers' bug
