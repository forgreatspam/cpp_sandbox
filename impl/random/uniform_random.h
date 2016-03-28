#pragma once
#include <atomic>
#include <limits> 
#include <mutex>

#include "impl/thread_mode.h"
#include "impl/declare_random_generator.h"
#include "impl/rnd_base.h"
#include "util/spinlock.h"


namespace rnd
{
  struct UniformRandom;


  typedef unsigned long ValueType;
  ValueType const DEFAULT_START = 1234567;
  ValueType const MULTIPLIER = 663608941;
  double const MAX_VALUE = static_cast<double>(std::numeric_limits<ValueType>::max());
  int const FORKED_VERSION_STEP = 400;


  namespace detail
  {
    inline ValueType GetNextOdd(ValueType val)
    {
      return val % 2 ? val : val + 1;
    }
  }


  struct UniformRandomSingleThreaded
  {
    UniformRandomSingleThreaded(ValueType start = DEFAULT_START) // !!! explicit
      : val_{ detail::GetNextOdd(start) }
    {}

    double operator()() noexcept
    {
      val_ *= MULTIPLIER;
      return val_ / MAX_VALUE;
    }

  private:
    ValueType val_;
  };


  template <class Mutex>
  struct UniformRandomWithMutex
  {
    explicit UniformRandomWithMutex(ValueType start = DEFAULT_START)
      : val_{ detail::GetNextOdd(start) }
    {}

    UniformRandomWithMutex(UniformRandomWithMutex const &) = delete;
    UniformRandomWithMutex& operator=(UniformRandomWithMutex const &) = delete;

    double operator()() noexcept
    {
      ValueType newVal;

      {
        std::lock_guard<Mutex> lock(mutex_);
        val_ *= MULTIPLIER;
        newVal = val_;
      }

      return newVal / MAX_VALUE;
    }

  private:
    Mutex mutex_;
    ValueType val_;
  };


  using UniformRandomSpinLock = UniformRandomWithMutex<util::SpinLockMutex>;


  struct UniformRandomLockFree
  {
    explicit UniformRandomLockFree(ValueType start = DEFAULT_START)
      : val_{ detail::GetNextOdd(start) }
    {}

    UniformRandomLockFree(UniformRandomLockFree const &) = delete;
    UniformRandomLockFree& operator=(UniformRandomLockFree const &) = delete;

    double operator()() noexcept
    {
      ValueType curVal = val_.load(std::memory_order_relaxed);
      while (true)
      {
        ValueType const newVal = curVal * MULTIPLIER;
        if (val_.compare_exchange_weak(curVal, newVal, std::memory_order_relaxed))
          return newVal / MAX_VALUE;
      }
    }

  private:
    std::atomic<ValueType> val_;
  };


  // Use one fork per thread. Using one fork from different threads leads to undefined behavior
  template <int N>
  struct UniformRandomForkable
  {
    typedef std::atomic<ValueType> AtomicValueType;

    explicit UniformRandomForkable(ValueType start = DEFAULT_START)
      : counter_{ N }
      , localVal_{ detail::GetNextOdd(start) }
      , sharedVal_{ std::make_shared<AtomicValueType>(localVal_) }
    {}

    UniformRandomForkable GetFork()
    {
      return UniformRandomForkable(sharedVal_);
    }

    double operator()() noexcept
    {
      if (!counter_--)
        ReserveSequence_();
      else
        localVal_ *= MULTIPLIER;
      return localVal_ / MAX_VALUE;
    }

  private:
    UniformRandomForkable(std::shared_ptr<AtomicValueType> srcShared)
      : counter_{ 0 }
      , localVal_{ 0 }
      , sharedVal_{ std::move(srcShared) }
    {}

    void ReserveSequence_()
    {
      ValueType curSharedVal = sharedVal_->load(std::memory_order_relaxed);
      do
      {
        localVal_ = curSharedVal * multiplierPow_;
      } while (sharedVal_->compare_exchange_weak(curSharedVal, localVal_, std::memory_order_relaxed));
      counter_ = N;
    }

    constexpr ValueType GetPow_(ValueType val, size_t n) const
    {
      return n ? val * GetPow_(val, n - 1) : 1;
    }

    ValueType const multiplierPow_ = GetPow_(MULTIPLIER, N + 1);

    size_t counter_;
    ValueType localVal_;
    std::shared_ptr<AtomicValueType> sharedVal_;
  };
}


BEGIN_DECLARE_RANDOM_GENERATOR(rnd::UniformRandom, rnd::RandomGeneratorPseudo)
  IMPLEMENTS(SingleThreaded, rnd::UniformRandomSingleThreaded),
  IMPLEMENTS(ThreadSafe, rnd::UniformRandomLockFree),  // or rnd::UniformRandomSpinLock
  IMPLEMENTS(Forkable, rnd::UniformRandomForkable<rnd::FORKED_VERSION_STEP>)
END_DECLARE_RANDOM_GENERATOR(rnd::UniformRandom)
