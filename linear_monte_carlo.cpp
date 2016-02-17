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
#include "impl/printers.h"
#include "impl/calculators.h"
#include "util/timer.h"


namespace bf = boost::fusion;
namespace mpl = boost::mpl;


template <class CalculatorIdentity, class Method>
decltype(auto) CreateCalculator(linear::Equation const & equation, Method method)
{
  // I cannot move this code into lambda body because it makes MSVC compiler crash
  using Calculator = typename mpl::apply<typename CalculatorIdentity::type, Method>::type;
  return Calculator(equation, method);
}

template <class CalculatorIdentity, class Methods>
void MakeExperimentsImpl(Methods methods, size_t minRepeat, size_t maxRepeat,
  std::ostream & stream = std::cout)
{
  stream << "Repeat ";
  bf::for_each(methods, [&stream](auto const & method) { stream << method << " "; });
  stream << std::endl;

  auto const equation = settings::GetEquation();

  auto calculators = bf::as_list(bf::transform(methods,
    [&equation](auto method)
    {
      return CreateCalculator<CalculatorIdentity>(equation, std::move(method));
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

struct MakeExperiments
{
  template <class CalculatorIdentity>
  void operator()(CalculatorIdentity)
  {
    auto methods = settings::GetMethods();
    std::ostream & stream = settings::GetStream();

    stream << ".";

    util::Timer timer;
    
    MakeExperimentsImpl<CalculatorIdentity>(
      methods, settings::MIN_REPEAT, settings::MAX_REPEAT, stream);

    std::cout << timer.GetTime() << "ms" << std::endl;
  }
};


int wmain(int argc, wchar_t * argv[])
{
  mpl::for_each<settings::Calculators>(MakeExperiments());

  return 0;
}

