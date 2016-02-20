#pragma once
#include <string>
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


namespace rnd
{
  // TODO: detect overhead which makes this calculator slower than async-based version
  template <class Method>
  class CalculatorThreadPool : public CalculatorBase<Method>
  {
  public:
    static int const MIN_CHUNK_PER_THREAD = 5000;

    // TODO: eliminate copy/paste
    CalculatorThreadPool(linear::Equation const & equation, Method method)
      : CalculatorBase<Method>(equation, std::move(method))
      , maxThreadCount_(std::max<int>(std::thread::hardware_concurrency(), 1))
      , threadPool_(std::make_shared<util::ThreadPool>(maxThreadCount_ - 1))
    {
      randomGenerators_.reserve(maxThreadCount_);
      auto randomGenerator = method_.CreateInstance<thread_mode::Forkable>();
      for (auto ii : util::range(maxThreadCount_ - 1))
        randomGenerators_.emplace_back(randomGenerator.GetFork());
      randomGenerators_.emplace_back(std::move(randomGenerator));
    }

    void Update(size_t curRepeat)
    {
      size_t const threadCount = std::max(1u, std::min(maxThreadCount_, curRepeat / MIN_CHUNK_PER_THREAD));

      typedef std::future<Estimate> EstimateFuture;
      std::vector<EstimateFuture> futures;
      futures.reserve(threadCount);

      size_t const chunkSize = curRepeat / threadCount;

      using Task = rnd::Estimate(RandomGenerator &, const linear::Equation &, size_t);
      using PackagedTask = std::packaged_task<Task>;

      for (auto ii : util::range(threadCount - 1))
      {
        auto task = PackagedTask(&rnd::GetEstimate<RandomGenerator>);
        futures.emplace_back(task.get_future());

        // std::function cannot be constructed from non-copyable functor
        // therefore, I cannot use just "task{ move(task) }" in lambda capture
        threadPool_->Post([this, task{ std::make_shared<PackagedTask>(move(task)) }, ii, chunkSize]() mutable {
            (*task)(std::ref(randomGenerators_[ii]), std::ref(equation_), chunkSize);
        });
      }

      auto task = PackagedTask(&rnd::GetEstimate<RandomGenerator>);
      futures.emplace_back(task.get_future());
      size_t const restRepeatCount = curRepeat - chunkSize * (threadCount - 1);
      task(std::ref(randomGenerators_[threadCount - 1]), std::ref(equation_), restRepeatCount);

      for (auto & future : futures)
      {
        auto const & newEstimate = future.get();

        estimate_.sum += newEstimate.sum;
        estimate_.sumSq += newEstimate.sumSq;
      }
      repeat_ += curRepeat;
    }

  private:
    unsigned const maxThreadCount_;
    using RandomGenerator = typename Method::template InstanceType<thread_mode::Forkable>;
    std::vector<RandomGenerator> randomGenerators_;
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
