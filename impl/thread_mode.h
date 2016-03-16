#pragma once
#include <mutex>
#include <memory>
#include <type_traits>

#include <boost/hana.hpp>

#include "util/traits.h"

namespace thread_mode
{
  struct SingleThreaded;
  struct ThreadSafe;
  struct Forkable;


  namespace mpl = boost::mpl;
  namespace hana = boost::hana;  // TODO: check namespace is declared inside other namespace


  template <class Algorithm>
  constexpr auto threadModes = hana::nothing;


  // TODO: make it more clear?
  template <class Algorithm, class Mode>
  using GetThreadMode = typename std::decay_t<decltype(threadModes<Algorithm>[hana::type_c<Mode>])>::type;


  // TODO: move to utils
  template <class T>
  class ThreadedFromSingle
  {
  public:
    ThreadedFromSingle() : singleThreaded_()
    {}

    ThreadedFromSingle(ThreadedFromSingle const & from)
      : singleThreaded_(from.singleThreaded)
    {}

    template <class Arg, class... Args, typename =
      std::enable_if_t<
        !util::IsCopyCtorArg_v<ThreadedFromSingle, Arg, Args...>
      >
    >
    ThreadedFromSingle(Arg&& arg, Args&&... args)
      : singleThreaded_(std::forward<Arg>(arg), std::forward<Args>(args)...)
    {}

    template <class... Args>
    decltype(auto) operator()(Args&&... args)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      return singleThreaded_(std::forward<Args>(args)...);
    }

  private:
    T singleThreaded_;
    std::mutex mutex_;
  };


  template <class T>
  class ForkedFromThreadSafe
  {
  public:
    using ThreadedPtr = std::shared_ptr<T>;

    ForkedFromThreadSafe()
      : impl_(std::make_shared<T>())
    {}

    ForkedFromThreadSafe(ForkedFromThreadSafe const & from)
      : impl_(from.impl_)
    {}

    ForkedFromThreadSafe(ForkedFromThreadSafe &&) = default;
    ForkedFromThreadSafe & operator=(ForkedFromThreadSafe &&) = default;

    template <class Arg, class... Args, typename =
      std::enable_if_t<
        !util::IsCopyCtorArg_v<ForkedFromThreadSafe, Arg, Args...>
      >
    >
    ForkedFromThreadSafe(Arg&& arg, Args&&... args)
      : impl_(std::make_shared<T>(std::forward<Arg>(arg), std::forward<Args>(args)...))
    {}

    decltype(auto) GetFork()
    {
      return ForkedFromThreadSafe(*this);
    }

    template <class... Args>
    decltype(auto) operator()(Args&&... args)
    {
      return impl_->operator()(std::forward<Args>(args)...);
    }

  private:
    ThreadedPtr impl_;
  };


  template <class T>
  using ForkedFromSingle = ForkedFromThreadSafe<ThreadedFromSingle<T>>;


  template <class Algorithm, class Impl>
  constexpr bool implements = hana::contains(hana::values(thread_mode::threadModes<Algorithm>), hana::type_c<Impl>);
}
