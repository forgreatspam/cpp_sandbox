#pragma once
#include <functional>
#include <list>
#include <thread>
#include <vector>

#include "unused.h"


namespace util
{
  // I am aware of boost's thread pool implementation. But normally this implemenation
  // is used together with boost::asio::io_service, which requires linkage with static library
  class ThreadPool
  {
  public:
    ThreadPool(size_t threadCount)
    {
      threads_.reserve(threadCount);
      for (auto _ UNUSED : util::range(threadCount))
        threads_.emplace_back(std::thread([this]() { RunWorker_(); }));
    }

    ThreadPool(ThreadPool const &) = delete;
    ThreadPool(ThreadPool &&) = default;
    ThreadPool & operator=(ThreadPool const &) = delete;
    ThreadPool & operator=(ThreadPool &&) = default;

    ~ThreadPool()
    {
      Post(Func());
      queueSignal_.notify_all();

      for (auto && thread : threads_)
        thread.join();
    }

    template <class AnyFunc>
    void Post(AnyFunc && func)
    {
      std::lock_guard<std::mutex> lock(queueMutex_);
      queue_.emplace_back(std::forward<AnyFunc>(func));
      queueSignal_.notify_one();
    }

  private:
    using Func = std::function<void()>;

    void RunWorker_()
    {
      std::unique_lock<std::mutex> lock(queueMutex_);
      while (true)
      {
        queueSignal_.wait(lock, [this]() mutable { return !queue_.empty(); });

        auto func = std::move(queue_.back());
        if (!func)
          return;

        queue_.pop_back();
        lock.unlock();

        func();
        lock.lock();
      }
    }

    std::condition_variable queueSignal_;
    std::mutex queueMutex_;
    std::vector<std::thread> threads_;
    std::list<Func> queue_;
  };
}
