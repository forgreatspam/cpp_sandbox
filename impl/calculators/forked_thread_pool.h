#pragma once
#include <string>
#include <thread>
#include <memory>
#include <vector>

#include "./calculators_fwd.h"
#include "./forked_common.h"
#include "impl/types.h"
#include "impl/estimate.h"
#include "impl/neumann_solver.h"
#include "util/range.h"
#include "util/thread_pool.h"


namespace rnd
{
  template <class Method>
  class CalculatorThreadPool : public ForkedCommon<Method>
  {
    using Base = ForkedCommon<Method>;

  public:
    CalculatorThreadPool(linear::Equation const & equation, Method method)
      : ForkedCommon<Method>(equation, std::move(method))
      , threadPool_(std::make_shared<util::ThreadPool>(Base::maxThreadCount_ - 1))
    {}

    void Update(size_t curRepeat)
    {
      size_t const threadCount = Base::GetThreadCount(curRepeat);

      typedef std::future<Estimate> EstimateFuture;
      std::vector<EstimateFuture> futures;
      futures.reserve(threadCount);

      size_t const chunkSize = curRepeat / threadCount;

      using Task = Estimate(typename Base::RandomGenerator &, const linear::Equation &, size_t);
      using PackagedTask = std::packaged_task<Task>;

      for (auto ii : util::range(threadCount - 1))
      {
        auto task = PackagedTask(&rnd::GetEstimate<typename Base::RandomGenerator>);
        futures.emplace_back(task.get_future());

        // std::function cannot be constructed from non-copyable functor
        // therefore, I cannot use just "task { move(task) }" in lambda capture
        threadPool_->Post([this, task = std::make_shared<PackagedTask>(move(task)), ii, chunkSize]() mutable {
            (*task)(std::ref(Base::randomGenerators_[ii]), std::ref(Base::equation_), chunkSize);
        });
      }

      auto task = PackagedTask(&rnd::GetEstimate<typename Base::RandomGenerator>);
      futures.emplace_back(task.get_future());
      size_t const restRepeatCount = curRepeat - chunkSize * (threadCount - 1);
      task(std::ref(Base::randomGenerators_[threadCount - 1]), std::ref(Base::equation_), restRepeatCount);

      Base::UpdateEstimate(std::move(futures));
      Base::repeat_ += curRepeat;
    }

  private:
    // TODO: allow calculators to be move-only, currently I have to use shared_ptr
    std::shared_ptr<util::ThreadPool> threadPool_;
  };


  template <>
  struct CalculatorName<CalculatorThreadPoolId>
  {
    static std::string Get()
    {
      return "forked using thread pool";
    }
  };
}
