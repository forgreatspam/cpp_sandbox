#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "impl/random/uniform_random_test.h"


int main(int argc, char** argv)
{
  ::testing::InitGoogleMock(& argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
