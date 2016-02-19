#pragma once
#include <string>

#include "./calculators_fwd.h"
#include "./common.h"
#include "impl/types.h"
#include "impl/estimate.h"
#include "impl/neumann_solver.h"


namespace rnd
{
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
}
