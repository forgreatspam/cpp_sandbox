#pragma once
#include "impl/types.h"
#include "impl/printers.h"


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


  template <class Printer, class Method, template <class Method> class Calculator>
  Printer & operator << (Printer & printer, Calculator<Method> const & calculator)
  {
    rnd::PrinterWrapper<Printer> printWrapper(printer);
    printWrapper(calculator.GetMethod().GetName(), calculator.GetEquation(),
      calculator.GetEstimate(), calculator.GetRepeat());
    return printer;
  }
}
