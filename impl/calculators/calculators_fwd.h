#pragma once
#include <boost/mpl/identity.hpp>
#include <boost/mpl/placeholders.hpp>

namespace mpl = boost::mpl;

namespace rnd
{
  template <class Method>
  class CalculatorSingleThreaded;

  template <class Method>
  class CalculatorForked;

  template <class Method>
  class CalculatorThreadPool;

  template <template <class> class T>
  using TemplateIdentity = mpl::identity<T<mpl::_1>>;;

  using CalculatorSingleThreadedId = TemplateIdentity<CalculatorSingleThreaded>;
  using CalculatorForkedId = TemplateIdentity<CalculatorForked>;
  using CalculatorThreadPoolId = TemplateIdentity<CalculatorThreadPool>;

  template <class CalculatorIdentity>
  struct CalculatorName;
}
