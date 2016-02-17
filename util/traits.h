#pragma once
#include <type_traits>

namespace util
{
    template<typename T>
    using BaseType = std::remove_cv_t<std::remove_reference_t<T>>;

    template <class T, class Arg, class... Args>
    struct IsCopyCtorArg
    {
      static bool const value = sizeof...(Args) == 0
                                && std::is_same<BaseType<Arg>, T>::value;
    };
}