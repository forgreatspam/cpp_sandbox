#pragma once
// Notice, boost has random numbers generator implementation, but it is
// not thread safe. They recommend to use mutexes which will work, but will cause
// significant performance loss in our case
#include <type_traits>

#include "util/traits.h"


namespace rnd
{
  template <class RandomFunc, class Enable = void>
  class RandomGenerator;  // To be specialized for each kind of random function


  template <class RandomFunc>
  class RandomGeneratorPseudo
  {
  public:
    RandomGeneratorPseudo(RandomFunc const & rndImpl)
      : rndImpl_(rndImpl)
    {}

    void NextVector() const
    {}

    double operator()()
    {
      return rndImpl_();
    }

    RandomGeneratorPseudo GetFork()
    {
      return RandomGeneratorPseudo(rndImpl_.GetFork());
    }

  private:
    RandomFunc rndImpl_;
  };


  template <class RandomFunc>
  class RandomGeneratorQuasi
  {
  public:
    RandomGeneratorQuasi(RandomFunc const & rndImpl)
      : RandomGeneratorQuasi(rndImpl, 0, 0)
    {}

    void NextVector()
    {
      ++row_;
      vectorNum_ = 0;
    }

    double operator()()
    {
      return rndImpl_(row_, vectorNum_++);
    }

    RandomGeneratorQuasi GetFork()
    {
      return RandomGeneratorQuasi(rndImpl_.GetFork(), vectorNum_, row_);
    }

  private:
    RandomGeneratorQuasi(RandomFunc const & rndImpl, size_t row, size_t vectorNum)
      : vectorNum_(vectorNum)
      , row_(row)
      , rndImpl_(rndImpl)
    {}

    size_t vectorNum_;
    size_t row_;
    RandomFunc rndImpl_;
  };


  template <class RandomFunc>
  decltype(auto) CreateGenerator(RandomFunc && randomFunc)
  {
    return RandomGenerator<std::decay_t<RandomFunc>>(std::forward<RandomFunc>(randomFunc));
  }
}