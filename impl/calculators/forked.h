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
#include "util/unused.h"


namespace rnd
{
  template <class Method>
  class CalculatorForked : public ForkedCommon<Method>
  {
    using Base = ForkedCommon<Method>;

  public:
    CalculatorForked(linear::Equation const & equation, Method method)
      : ForkedCommon<Method>(equation, std::move(method))
    {}

    void Update(size_t curRepeat)
    {
      unsigned const threadCount = Base::GetThreadCount(curRepeat);
      typedef std::future<Estimate> EstimateFuture;
      std::vector<EstimateFuture> futures;
      futures.reserve(threadCount);

      size_t const chunkSize = curRepeat / threadCount;
      auto randomGeneratorsIt = Base::GetRandomGenerators().begin();

      futures.emplace_back(std::async(std::launch::deferred, &rnd::GetEstimate<typename Base::RandomGenerator>,
        std::ref(*randomGeneratorsIt++), std::ref(Base::equation_), curRepeat - chunkSize * (threadCount - 1)));

      for (auto _ UNUSED : util::range(threadCount - 1))
        futures.emplace_back(std::async(std::launch::async, &rnd::GetEstimate<typename Base::RandomGenerator>,
          std::ref(*randomGeneratorsIt++), std::ref(Base::equation_), chunkSize));

      Base::UpdateEstimate(std::move(futures));
      Base::repeat_ += curRepeat;
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
