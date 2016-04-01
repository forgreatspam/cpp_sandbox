#pragma once
#include <type_traits>
#include <boost/hana.hpp>

namespace util
{
  namespace hana = boost::hana;

  template <class T1, class T2>
  constexpr auto MakeHanaPair = hana::make_pair(hana::type_c<T1>, hana::type_c<T2>);


  template <class T, class Arg, class... Args>
  constexpr bool isCopyCtorArg = sizeof...(Args) == 0 && std::is_same<std::decay_t<Arg>, T>::value;
}