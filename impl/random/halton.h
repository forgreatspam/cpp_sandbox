#pragma once
#include <vector>
#include <cmath>
#include <cassert>

#include "impl/thread_mode.h"
#include "impl/declare_random_generator.h"
#include "impl/rnd_base.h"

namespace rnd
{
  struct Halton;

  namespace detail
  {
    std::vector<size_t> const GetPrimes(unsigned maxPrime = 100'000)
    {
      std::vector<size_t> primes;
      primes.reserve(2 * static_cast<int>(maxPrime / std::log(maxPrime)));  // See the prime number theorem

      primes.push_back(2);
      for (size_t ii = 3; ii < maxPrime; ii += 2)
      {
        bool prime = true;
        for (size_t jj = 0; jj < primes.size() && primes[jj] * primes[jj] <= ii; ++jj)
        {
          if (ii % primes[jj] == 0)
          {
            prime = false;
            break;
          }
        }
        if (prime)
          primes.push_back(ii);
      }

      return primes; // return value optimization is expected
    }


    double GetHalton(size_t num, size_t prime)
    {
      double const mul = 1.0 / (double)prime;
      double alpha = 0;
      double mulPow = mul;

      while (num >= prime) {
        alpha += (num % prime) * mulPow;
        mulPow *= mul;
        num /= prime;  // TODO: use profiler here
      }
      alpha += num * mulPow;

      return alpha;
    }
  }


  struct GetHaltonByNumSingleThreaded
  {
    explicit GetHaltonByNumSingleThreaded(size_t startShift)
      : startShift_(startShift)
    {}

    double operator()(size_t num, size_t primePos) const
    {
      static auto const primes = detail::GetPrimes();

      assert(primePos < primes.size() && "Generate more primes");
      size_t const prime = primes[primePos];
      return detail::GetHalton(num + startShift_, prime);
    }

  private:
    size_t const startShift_;
  };


  struct GetHaltonByNumLockFree  // This class is used in implementation of forked version
  {
    explicit GetHaltonByNumLockFree(size_t startShift)
      : startShift_(startShift)
    {}

    double operator()(size_t num, size_t primePos) const
    {
      // yep, it might be expensive if threads are not reused
      static auto thread_local const primes = detail::GetPrimes();

      size_t const prime = primes[primePos];
      return detail::GetHalton(num + startShift_, prime);
    }

  private:
    size_t const startShift_;
  };
}


BEGIN_DECLARE_RANDOM_GENERATOR(rnd::Halton, rnd::RandomGeneratorQuasi)
  IMPLEMENTS(SingleThreaded, rnd::GetHaltonByNumSingleThreaded),
  IMPLEMENTS(ThreadSafe, rnd::GetHaltonByNumLockFree),
  IMPLEMENTS(Forkable, ForkedFromThreadSafe<rnd::GetHaltonByNumLockFree>)
END_DECLARE_RANDOM_GENERATOR(rnd::Halton)
