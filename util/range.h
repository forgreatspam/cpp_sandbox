#pragma once

#include <boost/range/irange.hpp>

namespace util
{
  template <class Integer>
  decltype(auto) range(Integer end)
  {
    return boost::irange(Integer(), end);
  }
}
