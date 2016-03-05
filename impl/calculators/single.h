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
    using Base = CalculatorBase<Method>;
  public:
    CalculatorSingleThreaded(linear::Equation const & equation, Method method)
      : CalculatorBase<Method>(equation, std::move(method))
      , randomGenerator_(method.template CreateInstance<thread_mode::SingleThreaded>())
    {}

    void Update(size_t curRepeat)
    {
      MakeEstimate(randomGenerator_, Base::equation_, curRepeat, Base::estimate_);
      Base::repeat_ += curRepeat;
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
