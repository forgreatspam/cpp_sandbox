#pragma once
#include <thread>
#include <future>
#include <vector>
#include <string>

#include "./calculators_fwd.h"
#include "./forked_common.h"
#include "impl/types.h"
#include "impl/estimate.h"
#include "impl/neumann_solver.h"
#include "util/range.h"


namespace rnd
{
  template <class Method>
  class CalculatorForked : public ForkedCommon<Method>
  {
  public:
    static int const MIN_CHUNK_PER_THREAD = 5000;

    CalculatorForked(linear::Equation const & equation, Method method)
      : ForkedCommon<Method>(equation, std::move(method))
    {}

    void Update(size_t curRepeat)
    {
      unsigned const threadCount = GetThreadCount(curRepeat);
      typedef std::future<Estimate> EstimateFuture;
      std::vector<EstimateFuture> futures;
      futures.reserve(threadCount);

      size_t const chunkSize = curRepeat / threadCount;

      futures.emplace_back(std::async(std::launch::deferred, &rnd::GetEstimate<RandomGenerator>,
        std::ref(randomGenerators_[threadCount - 1]), std::ref(equation_), curRepeat - chunkSize * (threadCount - 1)));

      for (auto ii : util::range(threadCount - 1))
        futures.emplace_back(std::async(std::launch::async, &rnd::GetEstimate<RandomGenerator>,
          std::ref(randomGenerators_[ii]), std::ref(equation_), chunkSize));

      UpdateEstimate(std::move(futures));
      repeat_ += curRepeat;
    }
  };


  template <>
  struct CalculatorName<CalculatorForkedId>
  {
    static std::string Get()
    {
      return "forked using std::async";
    }
  };
}
