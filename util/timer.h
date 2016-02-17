#pragma once
#include <chrono>


namespace util
{
  struct Timer
  {
    typedef std::chrono::high_resolution_clock Clock;
    typedef Clock::time_point TimePoint;

    Timer() : startTime_(Clock::now())
    {}

    double GetTime() const
    {
      using namespace std::chrono;

      TimePoint finishTime = Clock::now();
      auto msec = duration_cast<milliseconds>(finishTime - startTime_).count();
      return msec * 0.001;
    }

  private:
    TimePoint const startTime_;
  };
}