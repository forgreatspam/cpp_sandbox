#pragma once
#include <type_traits>
#include <boost/mpl/map.hpp>
#include "thread_mode.h"


namespace mpl = boost::mpl;


namespace rnd
{
  template <class RandomType>
  class GeneratorAdaptorStrategy;
}

#define BEGIN_DECLARE_RANDOM_GENERATOR(RandomType, GeneratorAdaptor) \
  namespace rnd \
  { \
    template <> \
    class GeneratorAdaptorStrategy<RandomType> \
    { \
      template <class RandomFunc> \
      using type = GeneratorAdaptor<RandomFunc>; \
    }; \
  } \
   \
  namespace thread_mode \
  { \
    template<> \
    struct ThreadModes<RandomType> \
      : boost::mpl::map <

#define IMPLEMENTS(ThreadMode, Impl) \
  mpl::pair<ThreadMode, Impl>

#define END_DECLARE_RANDOM_GENERATOR(RandomType) \
    > \
  {}; \
  } \
   \
  namespace rnd \
  { \
    template <class RandomFunc> \
    class RandomGenerator<RandomFunc, std::enable_if_t<thread_mode::Implements<RandomType, RandomFunc>::value>> \
      : public GeneratorAdaptorStrategy<RandomType>::type<RandomFunc> \
    {}; \
  }
