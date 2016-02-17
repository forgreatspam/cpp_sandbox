#pragma once
#include <iostream>
#include <tuple>
#include <type_traits>

#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/functional/value_factory.hpp>
#include <boost/mpl/identity.hpp>

#include "thread_mode.h"
#include "util/create_instance.h"


// The module provides CreateMethod method which returns
// a factory for generating of 'method' depending on threading module provided
// at compile time.

namespace rnd
{
  template <class Algorithm, class... Args>
  struct Method
  {
    Method(std::string name, Args && ... args)
      : name_(std::move(name))
      , args_(std::forward<Args>(args)...)
    {}

    template <class ThreadMode>
    decltype(auto) CreateInstance() const
    {
      decltype(auto) randomFunc = util::CreateInstance<thread_mode::GetThreadMode<Algorithm, ThreadMode>>(args_);
      return CreateGenerator(std::move(randomFunc));
    }

    template <class ThreadMode>
    using InstanceType = decltype(std::declval<Method>().CreateInstance<ThreadMode>());

    decltype(auto) GetName() const
    {
      return GetName_(mpl::identity<Algorithm>());
    }

  private:
    template <class Algorithm_>
    std::string const & GetName_(mpl::identity<Algorithm_>) const
    {
      return name_;
    }

    template <int arg, template <int> class Algorithm_>
    std::string GetName_(mpl::identity<Algorithm_<arg>>) const
    {
      return name_ + "<" + std::to_string(arg) + ">";
    }

    std::string const name_;
    std::tuple<Args...> const args_;
  };


  template <class Algorithm, class... Args>
  auto CreateMethod(std::string name, Args &&... args )
  {
    return Method<Algorithm, Args...>(std::move(name), std::forward<Args>(args)... );
  }


  template <class Algorithm, class... Args>
  std::ostream & operator << (std::ostream & stream, Method<Algorithm, Args...> const & method)
  {
    return stream << method.GetName();
  }
}