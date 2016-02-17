#pragma once
#include <atomic>


namespace util
{
  // Yes, I am aware of boost spinlock, but it is a test polygon, not the production code
  // And as experiments show, for our case this class works faster than std::mutex
  struct SpinLockMutex
  {
    SpinLockMutex() : flag_{ ATOMIC_FLAG_INIT }
    {}

    SpinLockMutex(SpinLockMutex const &) = delete;
    SpinLockMutex & operator=(SpinLockMutex const &) = delete;

    void lock()
    {
      while (flag_.test_and_set(std::memory_order_acquire))
        ;
    }

    void unlock()
    {
      flag_.clear(std::memory_order_release);
    }

  private:
    std::atomic_flag flag_;
  };
}
