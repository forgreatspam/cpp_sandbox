#pragma once
#include <type_traits>
#include <boost/mpl/map.hpp>
#include "thread_mode.h"


namespace mpl = boost::mpl;


namespace rnd
{
  template <class RandomType, class RandomFunc>
  struct GeneratorAdaptorStrategy_;

  template <class RandomType, class RandomFunc>
  using GeneratorAdaptorType_ = typename GeneratorAdaptorStrategy_<RandomType, RandomFunc>::type;
}


#define BEGIN_DECLARE_RANDOM_GENERATOR(RandomType, GeneratorAdaptor) \
  namespace rnd \
  { \
    template <class RandomFunc> \
    struct GeneratorAdaptorStrategy_<RandomType, RandomFunc> \
    { \
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
    struct RandomGenerator<RandomFunc, std::enable_if_t<thread_mode::Implements<RandomType, RandomFunc>::value>> \
      : public GeneratorAdaptorType_<RandomType, RandomFunc> \
    { \
      template <class... Args> \
      RandomGenerator(Args && ... args) : \
        GeneratorAdaptorType_<RandomType, RandomFunc>(std::forward<Args>(args)...) \
      {} \
    }; \
  }
