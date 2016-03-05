#pragma once
#include <numeric>
#include <cmath>

#include "types.h"
#include "estimate.h"
#include "util/range.h"
#include "util/unused.h"


namespace rnd { namespace
{
  template <class RandomGenerator>
  void MakeEstimate(RandomGenerator & randomGenerator, linear::Equation const & equation,
                    size_t const repeat, Estimate & estimate)
  {
    double const continueProbability = 0.95;

    // Notice, that loops order is essential for quasi-random numbers
    // The estimate parameters are not optimal and do not even tend to be optimal
    for (auto jj: util::range(linear::Equation::dimension))
    {
      for (auto _ UNUSED: util::range(repeat))
      {
        double estimateInPoint = 1;
        size_t pos = jj;

        while (true)
        {
          double const randomVal = randomGenerator();
          if (randomVal > continueProbability)
          {
            estimateInPoint *= equation.freeMember[pos] / (1. - continueProbability);
            randomGenerator.NextVector();
            break;
          }
          else {
            size_t const newPos = static_cast<size_t>(randomVal * linear::Equation::dimension / continueProbability);
            estimateInPoint *= equation.matrix[pos][newPos] * linear::Equation::dimension / continueProbability;
            pos = newPos;
          }
        }

        estimate.sum[jj] += estimateInPoint;
        estimate.sumSq[jj] += estimateInPoint * estimateInPoint;
      }
    }
  }


  template <class RandomGenerator>
  Estimate GetEstimate(RandomGenerator & randomGenerator, linear::Equation const & equation, size_t repeat)
  {
    Estimate estimate;
    MakeEstimate(randomGenerator, equation, repeat, estimate);
    return estimate;
  }
}}
