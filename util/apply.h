#pragma once
#include <tuple>
#include <functional>
#include <utility>
#include <type_traits>


namespace util
{
  namespace detail
  {
    // see http://en.cppreference.com/w/cpp/experimental/apply
    template <class F, class Tuple, std::size_t... I>
    constexpr decltype(auto) ApplyImpl(F && f, Tuple && t, std::index_sequence<I...>)
    {
      // Notice, forwarding of 't' is fine here because only one tuple's element is moved at a time
      return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);

      // std::invoke is not supported by gcc 5.2
      //return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
    }
  }


  // TODO: rewrite using boost::hana::fuse
  template <class F, class Tuple>
  constexpr decltype(auto) Apply(F && f, Tuple && t)
  {
    return detail::ApplyImpl(std::forward<F>(f), std::forward<Tuple>(t),
      std::make_index_sequence <
      std::tuple_size<std::decay_t<Tuple>>{}()
      > {});
  }
}
