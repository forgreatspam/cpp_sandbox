#pragma once
#include "types.h"


namespace rnd
{
  struct Estimate
  {
    linear::Vector sum {};
    linear::Vector sumSq {};
  };
}