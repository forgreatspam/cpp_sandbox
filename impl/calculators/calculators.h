#pragma once
#include <thread>
#include <future>
#include <vector>
#include <iostream>
#include <string>

#include "calculators_fwd.h"
#include "printers.h"
#include "types.h"
#include "estimate.h"
#include "neumann_solver.h"
#include "util/range.h"


namespace rnd
{
  template <class Method>
  class CalculatorBase
  {
  public:
    CalculatorBase(linear::Equation const & equation, Method method)
      : equation_(equation)
      , method_(std::move(method))
    {}

    linear::Equation const & GetEquation() const
    {
      return equation_;
    }

    size_t GetRepeat() const
    {
      return repeat_;
    }

    Method const & GetMethod() const
    {
      return method_;
    }

    Estimate const & GetEstimate() const
    {
      return estimate_;
    }

  protected:
    linear::Equation const & equation_;
    Method const method_;
    Estimate estimate_;
    size_t repeat_ = 0;
  };


  template <class Method>
  class CalculatorSingleThreaded : public CalculatorBase<Method>
  {
  public:
    CalculatorSingleThreaded(linear::Equation const & equation, Method method)
      : CalculatorBase<Method>(equation, std::move(method))
      , randomGenerator_(method.CreateInstance<thread_mode::SingleThreaded>())
    {}

    void Update(size_t curRepeat)
    {
      MakeEstimate(randomGenerator_, equation_, curRepeat, estimate_);
      repeat_ += curRepeat;
    }

  private:
    using RandomGenerator = typename Method::template InstanceType<thread_mode::SingleThreaded>;
    RandomGenerator randomGenerator_;
  };


  template <>
  struct CalculatorName<CalculatorSingleThreadedId>
  {
    static std::string Get()
    {
      return "single threaded";
    }
  };


  template <class Method>
  class CalculatorForked : public CalculatorBase<Method>
  {
  public:
    static int const MIN_CHUNK_PER_THREAD = 10000;

    CalculatorForked(linear::Equation const & equation, Method method)
      : CalculatorBase<Method>(equation, std::move(method))
      , maxThreadCount_(std::max<int>(std::thread::hardware_concurrency(), 1))
    {
      randomGenerators_.reserve(maxThreadCount_);
      auto randomGenerator = method_.CreateInstance<thread_mode::Forkable>();
      for (auto ii: util::range(maxThreadCount_ - 1))
        randomGenerators_.emplace_back(randomGenerator.GetFork());
      randomGenerators_.emplace_back(std::move(randomGenerator));
    }

    void Update(size_t curRepeat)
    {
      unsigned const threadCount = std::max(1u, std::min(maxThreadCount_, curRepeat / MIN_CHUNK_PER_THREAD));
      typedef std::future<Estimate> EstimateFuture;
      std::vector<EstimateFuture> futures;
      futures.reserve(threadCount);

      size_t const chunkSize = curRepeat / threadCount;

      for (auto ii: util::range(threadCount - 1))
        futures.emplace_back(std::async(std::launch::async, &rnd::GetEstimate<RandomGenerator>,
          std::ref(randomGenerators_[ii]), std::ref(equation_), chunkSize));

      futures.emplace_back(std::async(std::launch::deferred, &rnd::GetEstimate<RandomGenerator>,
        std::ref(randomGenerators_[threadCount - 1]), std::ref(equation_), curRepeat - chunkSize * (threadCount - 1)));

      for (auto & future: futures)
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
  };


  template <>
  struct CalculatorName<CalculatorForkedId>
  {
    static std::string Get()
    {
      return "forked";
    }
  };


  template <class Printer, class Method, template <class Method> class Calculator>
  Printer & operator << (Printer & printer, Calculator<Method> const & calculator)
  {
    rnd::PrinterWrapper<Printer> printWrapper(printer);
    printWrapper(calculator.GetMethod().GetName(), calculator.GetEquation(),
      calculator.GetEstimate(), calculator.GetRepeat());
    return printer;
  }
}