#pragma once

#include <numeric>
#include <vector>

#include "settings.h"
#include "util/range.h"
#include "util/zip.h"


namespace linear
{
  template <class T, size_t initialSize> struct ResizedVector;


  typedef ResizedVector<double, settings::DIMENSION> Vector;
  typedef ResizedVector<Vector, settings::DIMENSION> Matrix;


  template <class T, size_t initialSize>
  struct ResizedVector : public std::vector<T>
  {
    using Base = std::vector<T>;

    ResizedVector() : Base(initialSize)
    {}

    ResizedVector & operator+=(ResizedVector const & add)
    {
      for (auto & x : util::zip(*this, add))
        x[0] += x[1];
      return *this;
    }
  };


  struct Equation
  {
    Equation(Matrix const & matrix, Vector const & freeMember)
      : matrix(matrix)
      , freeMember(freeMember)
    {}

    Equation(Matrix && matrix, Vector && freeMember)
      : matrix(std::move(matrix))
      , freeMember(std::move(freeMember))
    {}

    static size_t const dimension = settings::DIMENSION;
    Matrix const matrix;
    Vector const freeMember;
  };


  decltype(auto) GetNorm(Vector const & vect)
  {
    auto const sumSq = std::accumulate(vect.cbegin(), vect.cend(), 0.,
      [](auto sumSq, auto x) { return sumSq + x*x; });
    return std::sqrt(sumSq);
  }


  Vector GetDiscrepancy(Equation const & equation, Vector const & estimate)
  {
    Vector matrixMulSolution{};

    for (auto ii : util::range(settings::DIMENSION))
    {
      for (auto jj : util::range(settings::DIMENSION))
        matrixMulSolution[ii] += equation.matrix[ii][jj] * estimate[jj];
    }

    Vector discrepancy{};
    for (auto ii : util::range(settings::DIMENSION))
      discrepancy[ii] = estimate[ii] - matrixMulSolution[ii] - equation.freeMember[ii];

    return discrepancy;
  }
}