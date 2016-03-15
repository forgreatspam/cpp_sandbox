#include <boost/mpl/for_each.hpp>

#include "settings/settings.h"
#include "settings/equation.h"
#include "impl/make_experiments.h"


namespace mpl = boost::mpl;


#if defined (_MSC_VER)
int wmain(int argc, wchar_t * argv[])
#else
int main(int argc, char * argv[])
#endif
{
  mpl::for_each<settings::Calculators>(rnd::MakeExperiments(settings::GetEquation()));

  return 0;
}
