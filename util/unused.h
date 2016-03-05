#pragma once


// see http://stackoverflow.com/questions/21792347/unnamed-loop-variable-in-range-based-for-loop
#if defined(__GNUC__)
#  define UNUSED __attribute__ ((unused))
#else
#  define UNUSED
#endif
