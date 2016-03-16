#pragma once
#include <type_traits>

#include <boost/hana.hpp>

#include "impl/thread_mode.h"
#include "./uniform_random.h"
#include "./halton.h"
#include "./sobol.h"
#include "util/traits.h"

// Notice, there is no scientific reason to use these 'methods', but they provided quite
// a good task for using MPL

namespace hana = boost::hana;


namespace rnd
{
  int const MAX_BITS = 10;

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
        !util::isCopyCtorArg<Randomized, Arg, Args...>
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
            class ForkedImpl = ForkedFromSingle<SingleImpl>>
  constexpr auto threadModeHelper = hana::make_map(
      hana::make_pair(hana::type_c<SingleThreaded>, hana::type_c<SingleImpl>)
    , hana::make_pair(hana::type_c<ThreadSafe>, hana::type_c<ThreadedImpl>)
    , hana::make_pair(hana::type_c<Forkable>, hana::type_c<ForkedImpl>));


  template <int bits>
  constexpr auto threadModes<rnd::RandomizedHalton<bits>> =
      threadModeHelper<rnd::Randomized<rnd::Halton, thread_mode::SingleThreaded, bits>,
                       rnd::Randomized<rnd::Halton, thread_mode::ThreadSafe, bits>,
                       rnd::Randomized<rnd::Halton, thread_mode::Forkable, bits>>;


  template <int bits>
  constexpr auto threadModes<rnd::RandomizedSobol<bits>> =
      threadModeHelper<rnd::Randomized<rnd::Sobol, thread_mode::SingleThreaded, bits>,
                       rnd::Randomized<rnd::Sobol, thread_mode::ThreadSafe, bits>,
                       rnd::Randomized<rnd::Sobol, thread_mode::Forkable, bits>>;
}


namespace rnd
{
  namespace detail
  {
    constexpr auto bitsRange = hana::to_tuple(hana::make_range(hana::int_c<0>, hana::int_c<MAX_BITS + 1>));

    template <template <int> class Algorithm, class RandomFunc>
    struct Implements  // NOTICE: constexpr lambdas are not supported yet, functions cannot accept auto parameter :(
    {
      template <class IntegralConst>
      constexpr bool operator()(IntegralConst) const
      {
        return thread_mode::implements<Algorithm<IntegralConst::value>, RandomFunc>;
      }
    };

    template <template <int> class Algorithm, class RandomFunc>
    constexpr auto anyImplements = hana::any(hana::transform(bitsRange, Implements<Algorithm, RandomFunc>{}));
  }

  template <class RandomFunc>
  struct RandomGenerator<RandomFunc, std::enable_if_t<
    detail::anyImplements<RandomizedHalton, RandomFunc> || detail::anyImplements<RandomizedSobol, RandomFunc>>>
    : public RandomGeneratorQuasi<RandomFunc>
  {
    template <class... Args>
    RandomGenerator(Args && ... args)
      : RandomGeneratorQuasi<RandomFunc>(std::forward<Args>(args)...)
    {}
  };
}