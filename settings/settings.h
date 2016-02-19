#pragma once

#include <iostream>

#include <boost/fusion/container/list.hpp>
#include <boost/fusion/include/make_list.hpp>
#include <boost/mpl/list.hpp>

#include "impl/halton.h"
#include "impl/sobol.h"
#include "impl/rnd_base.h"
#include "impl/method.h"
#include "impl/uniform_random.h"
#include "impl/randomized_quasi.h"
#include "impl/calculators_fwd.h"
#include "util/stream_utils.h"


namespace settings
{
  namespace mpl = boost::mpl;

  size_t const DIMENSION = 20;
  size_t const MIN_REPEAT = 10;
  size_t const MAX_REPEAT = 25'000;

  char const * LOG_FILE_NAME = "result.txt";


  using Calculators = mpl::list<
    rnd::CalculatorSingleThreadedId,
    rnd::CalculatorForkedId
  >::type;


  decltype(auto) GetMethods()
  {
    using namespace boost::fusion;

    return make_list(
        rnd::CreateMethod<rnd::UniformRandom>("Pseudo", 36'543)
      , rnd::CreateMethod<rnd::Halton>("Halton", 10'000)
      , rnd::CreateMethod<rnd::Sobol>("Sobol")
      , rnd::CreateMethod<rnd::RandomizedHalton<2>>("RndHalton", 10'000)
      , rnd::CreateMethod<rnd::RandomizedSobol<2>>("RndSobol")
      );
  }


  std::ostream & GetStream()
  {
    return util::GetFileAndConsoleStream(LOG_FILE_NAME);
  }
}
