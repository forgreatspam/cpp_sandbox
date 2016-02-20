#pragma once
#include <type_traits>

#include <boost/mpl/map.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/count_if.hpp>

#include "impl/thread_mode.h"
#include "./uniform_random.h"
#include "./halton.h"
#include "./sobol.h"
#include "util/traits.h"

// Notice, there is no scientific reason to use these 'methods', but they provided quite
// a good task for using MPL

namespace mpl = boost::mpl;


namespace rnd
{
  template <int bits>
  struct RandomizedHalton;


  template <int bits>
  struct RandomizedSobol;


  template <class Algorithm, class ThreadMode, int bits>
  struct Randomized
  {
    using AlgorithmMode = thread_mode::GetThreadMode<Algorithm, ThreadMode>;
    using UniformRandom = thread_mode::GetThreadMode<rnd::UniformRandom, ThreadMode>;

    Randomized(Randomized const & from) = default;
    Randomized(Randomized && from) = default;
    Randomized() = default;

    template <class Arg, class... Args, typename = 
      std::enable_if_t<
        !util::IsCopyCtorArg<Randomized, Arg, Args...>::value
      >
    >
    Randomized(Arg && arg, Args &&... args)
      : algorithm_(std::forward<Arg>(arg), std::forward<Args>(args)...)
    {}


    template <class... Args>
    double operator()(Args && ... args)
    {
      return MixRandom_(algorithm_(std::forward<Args>(args)...));
    }

    Randomized GetFork()
    {
      return Randomized(algorithm_.GetFork(), uniformRandom_.GetFork());
    }

  private:
    Randomized(AlgorithmMode && algorithm, UniformRandom && uniformRandom)
      : algorithm_(algorithm)
      , uniformRandom_(std::move(uniformRandom))
    {}

    double MixRandom_(double x) noexcept
    {
      unsigned const mul = 1 << bits;
      return (static_cast<int>(x * mul) + uniformRandom_()) / mul;
    }

    AlgorithmMode algorithm_;
    UniformRandom uniformRandom_;
  };
}


namespace thread_mode
{
  template <class SingleImpl,
            class ThreadedImpl = ThreadedFromSingle<SingleImpl>,
            class ForkedImpl = ForkedFromSingleThreaded<SingleImpl>>
  using ThreadModeHelper = mpl::map<mpl::pair<SingleThreaded, SingleImpl>
                                   , mpl::pair<ThreadSafe, ThreadedImpl>
                                   , mpl::pair<Forkable, ForkedImpl>>;


  template <int bits>
  struct ThreadModes<rnd::RandomizedHalton<bits>>
    : ThreadModeHelper<rnd::Randomized<rnd::Halton, thread_mode::SingleThreaded, bits>,
               rnd::Randomized<rnd::Halton, thread_mode::ThreadSafe, bits>,
               rnd::Randomized<rnd::Halton, thread_mode::Forkable, bits >>
  {};


  template <int bits>
  struct ThreadModes<rnd::RandomizedSobol<bits>>
    : ThreadModeHelper<rnd::Randomized<rnd::Sobol, thread_mode::SingleThreaded, bits>,
               rnd::Randomized<rnd::Sobol, thread_mode::ThreadSafe, bits>,
               rnd::Randomized<rnd::Sobol, thread_mode::Forkable, bits >>
  {};
}


namespace rnd
{
  int const MAX_BITS = 10;


  template <template <int> class Algorithm, class RandomFunc>
  struct ImplementsWrapper
  {
    template <class Num>
    struct Result: public thread_mode::Implements<Algorithm<Num::value>, RandomFunc>
    {};
  };


  using VectorInserter = mpl::back_inserter<mpl::vector<>>;


  using BitsRange = mpl::range_c<int, 0, MAX_BITS>;


  template <template <int> class Algorithm, class RandomFunc>
  using ImplementsValList =
    typename mpl::transform<BitsRange, typename ImplementsWrapper<Algorithm, RandomFunc>::Result<mpl::_1>, VectorInserter>::type;


  template <template <int> class Algorithm, class RandomFunc>
  struct IsTrue
  {
    template <class T>
    struct Result : public T
    {};
  };


  template <template <int> class Algorithm, class RandomFunc>
  class AnyImplements
  {
    // QUESTION: why this member is visible in template argument expression?
    static bool const value = mpl::count_if<ImplementsValList<Algorithm, RandomFunc>,
      typename IsTrue<Algorithm, RandomFunc>::Result<mpl::_1 >>::type::value > 0;
  };

  template <class RandomFunc>
  struct RandomGenerator<RandomFunc, std::enable_if_t<
    AnyImplements<RandomizedHalton, RandomFunc>::value
      || AnyImplements<RandomizedSobol, RandomFunc>::value>>
    : public RandomGeneratorQuasi<RandomFunc>
  {
    template <class... Args>
    RandomGenerator(Args && ... args)
      : RandomGeneratorQuasi<RandomFunc>(std::forward<Args>(args)...)
    {}
  };
}