#pragma once
#include <thread>
#include <memory>
#include <vector>

#include "./calculators_fwd.h"
#include "./common.h"
#include "impl/types.h"
#include "impl/estimate.h"
#include "impl/neumann_solver.h"
#include "util/range.h"
#include "util/thread_pool.h"
#include "util/unused.h"


namespace rnd
{
  template <class Method>
  class ForkedCommon: public CalculatorBase<Method>
  {
    using Base = CalculatorBase<Method>;
  public:
    static int const MIN_CHUNK_PER_THREAD = 5000;

    ForkedCommon(linear::Equation const & equation, Method method)
      : CalculatorBase<Method>(equation, std::move(method))
      , maxThreadCount_(std::max<int>(std::thread::hardware_concurrency(), 1))
    {
      randomGenerators_.reserve(maxThreadCount_);
      auto randomGenerator = Base::method_.template CreateInstance<thread_mode::Forkable>();
      for (auto _ UNUSED: util::range(maxThreadCount_ - 1))
        randomGenerators_.emplace_back(randomGenerator.GetFork());
      randomGenerators_.emplace_back(std::move(randomGenerator));
    }

  protected:
    size_t GetThreadCount(size_t repeat) const
    {
      return std::max<size_t>(1u, std::min<size_t>(maxThreadCount_, repeat / MIN_CHUNK_PER_THREAD));
    }

    template <class Futures>
    void UpdateEstimate(Futures && futures)
    {
      for (auto & future : futures)
      {
        auto const & newEstimate = future.get();

        Base::estimate_.sum += newEstimate.sum;
        Base::estimate_.sumSq += newEstimate.sumSq;
      }
    }

    unsigned const maxThreadCount_;
    using RandomGenerator = typename Method::template InstanceType<thread_mode::Forkable>;
    std::vector<RandomGenerator> randomGenerators_;
  };
}
