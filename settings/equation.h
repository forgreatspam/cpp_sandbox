#pragma once
#include <boost/range/irange.hpp>

#include "impl/types.h"

namespace settings
{
  linear::Equation GetEquation()
  {
    // For our example it is not important how to generate equation unless the matrix is positive-definite
    // and all its eugene values are less than 1..., well 0.95 becase of hardcoded adsorption
    // probability in Neumann-Ulam scheme
    int tmpValue = 0;

    linear::Matrix matrix;
    for (auto & row : matrix)
      for (auto & element : row)
        element = (tmpValue++ % 4) * 0.2 / matrix.size();

    linear::Vector freeMember;
    for (auto & element : freeMember)
      element = tmpValue++ % 10;

    return linear::Equation(std::move(matrix), std::move(freeMember));
  }
}