#pragma once

#include <iostream>

#include <boost/fusion/container/list.hpp>
#include <boost/fusion/include/make_list.hpp>
#include <boost/mpl/list.hpp>

#include "impl/random/halton.h"
#include "impl/random/sobol.h"
#include "impl/random/uniform_random.h"
#include "impl/random/randomized_quasi.h"
#include "impl/rnd_base.h"
#include "impl/method.h"
#include "impl/calculators/calculators_fwd.h"
#include "util/stream_utils.h"


namespace settings
{
  namespace mpl = boost::mpl;


  size_t const DIMENSION = 20;
  size_t const MIN_REPEAT = 10;
  size_t const MAX_REPEAT = 100'000;

  char const * LOG_FILE_NAME = "result.txt";


  // NOTICE: Presumably being a 'superset' of mpl and fusion, hana does not have template placeholders.
  using Calculators = mpl::list<
      rnd::CalculatorSingleThreadedId
    , rnd::CalculatorForkedId
    , rnd::CalculatorThreadPoolId
  >::type;


  decltype(auto) GetMethods()
  {
    using namespace boost::fusion;

    return make_list(
        rnd::CreateMethod<rnd::UniformRandom>("Pseudo", 36'543)
      , rnd::CreateMethod<rnd::Halton>("Halton", 10'000)
      //, rnd::CreateMethod<rnd::Sobol>("Sobol")  // This method is too slow
      , rnd::CreateMethod<rnd::RandomizedHalton<2>>("RndHalton", 10'000)
      //, rnd::CreateMethod<rnd::RandomizedSobol<2>>("RndSobol")  // This method is too slow
      );
  }


  std::ostream & GetStream()
  {
    return util::GetFileAndConsoleStream(LOG_FILE_NAME);
  }
}
