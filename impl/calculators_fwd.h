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

  template <template <class> class T>
  using TemplateIdentity = mpl::identity<T<mpl::_1>>;;

  using CalculatorSingleThreadedId = TemplateIdentity<CalculatorSingleThreaded>;
  using CalculatorForkedId = TemplateIdentity<CalculatorForked>;

  template <class CalculatorIdentity>
  struct CalculatorName;
}
