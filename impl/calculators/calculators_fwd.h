#pragma once
#include <boost/mpl/identity.hpp>
#include <boost/mpl/placeholders.hpp>


namespace rnd
{
  namespace mpl = boost::mpl;


  template <class Method>
  class CalculatorSingleThreaded;

  template <class Method>
  class CalculatorForked;

  template <class Method>
  class CalculatorThreadPool;

  template <template <class> class T>
  using TemplateIdentity = mpl::identity<T<mpl::_1>>;;

  using CalculatorSingleThreadedId = TemplateIdentity<CalculatorSingleThreaded>;  // TODO: replace with hana::type_c, see hana::make_type
  using CalculatorForkedId = TemplateIdentity<CalculatorForked>;
  using CalculatorThreadPoolId = TemplateIdentity<CalculatorThreadPool>;

  template <class CalculatorIdentity>
  struct CalculatorName;
}
