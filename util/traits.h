#pragma once
#include <type_traits>

namespace util
{
    template <class T, class Arg, class... Args>
    constexpr bool isCopyCtorArg = sizeof...(Args) == 0 && std::is_same<std::decay_t<Arg>, T>::value;
}