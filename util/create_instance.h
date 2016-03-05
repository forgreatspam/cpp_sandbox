#pragma once
#include <utility>
#include <functional>
#include <tuple>
#include <type_traits>


// The module provides
//    template <class T, class... Args>
//    decltype(auto) CreateInstance(std::tuple<Args...> const & args)
// method which returns a new instance of class T created with arguments provided
// in form of typle-compatible object

namespace util
{
  // TODO: move to detail namespace

  // see http://en.cppreference.com/w/cpp/experimental/apply
  template <class F, class Tuple, std::size_t... I>
  constexpr decltype(auto) ApplyImpl(F && f, Tuple && t, std::index_sequence<I...>)
  {
    // Notice, forwarding of 't' is fine here because only one tuple's element is moved at a time
    return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);

    // std::invoke is not supported by gcc 5.2
    //return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
  }


  template <class F, class Tuple>
  constexpr decltype(auto) Apply(F && f, Tuple && t)
  {
    return ApplyImpl(std::forward<F>(f), std::forward<Tuple>(t),
      std::make_index_sequence<
        std::tuple_size<std::decay_t<Tuple>>{}()
      >{});
  }


  template <class T>
  struct CreateInstanceImpl
  {
    template <class... Args>
    decltype(auto) operator()(Args&&... args) const
    {
      return T(std::forward<Args>(args)...);
    }
  };

  
  template <class T, class... Args>
  decltype(auto) CreateInstance(std::tuple<Args...> const & args)
  {
    return Apply(CreateInstanceImpl<T>(), args);
  }
}