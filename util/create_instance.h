#pragma once
#include <tuple>

#include "apply.h"

// The module provides
//    template <class T, class... Args>
//    decltype(auto) CreateInstance(std::tuple<Args...> const & args)
// method which returns a new instance of class T created with arguments provided
// in form of typle-compatible object

namespace util
{
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