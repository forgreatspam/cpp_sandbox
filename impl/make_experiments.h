#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <future>

#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/as_list.hpp>
#include <boost/mpl/for_each.hpp>

#include "settings/settings.h"
#include "settings/equation.h"
#include "calculators/single.h"
#include "calculators/forked.h"
#include "calculators/forked_thread_pool.h"
#include "printers.h"
#include "util/timer.h"


namespace bf = boost::fusion;
namespace mpl = boost::mpl;

namespace rnd
{
  template <class CalculatorIdentity, class Methods>
  void MakeExperimentsImpl(linear::Equation const & equation, Methods methods, size_t minRepeat, size_t maxRepeat,
    std::ostream & stream = std::cout)
  {
    stream << "Calculator type: " << CalculatorName<CalculatorIdentity>::Get() << std::endl;

    stream << "Repeat ";
    bf::for_each(methods, [&stream](auto const & method) { stream << method << " "; });
    stream << std::endl;

    auto calculators = bf::as_list(bf::transform(methods,
      [&equation](auto method)
      {
        using MethodType = decltype(method);
        using Calculator = typename mpl::apply<typename CalculatorIdentity::type, MethodType>::type;
        return Calculator(equation, method);
      }));

    for (size_t repeat = minRepeat; repeat <= maxRepeat; repeat *= 2)
    {
      stream << repeat << " ";
      bf::for_each(calculators, [repeat](auto & calculator) { calculator.Update(repeat - calculator.GetRepeat()); });

      auto printer = rnd::DiscrepancyNormPrinter(stream);
      bf::for_each(calculators, [&printer](auto const & calculator) { printer << calculator; });

      stream << std::endl;
    }
  }

  class MakeExperiments
  {
  public:
    MakeExperiments(linear::Equation && equation)
      : equation_(std::move(equation))
    {}

    MakeExperiments(linear::Equation const & equation)
      : equation_(equation)
    {}

    template <class CalculatorIdentity>
    void operator()(CalculatorIdentity) const
    {
      auto methods = settings::GetMethods();
      std::ostream & stream = settings::GetStream();

      util::Timer timer;

      MakeExperimentsImpl<CalculatorIdentity>(
        equation_, methods, settings::MIN_REPEAT, settings::MAX_REPEAT, stream);

      stream << timer.GetTime() << "ms" << std::endl << std::endl;
    }

  private:
    linear::Equation const equation_;
  };
}