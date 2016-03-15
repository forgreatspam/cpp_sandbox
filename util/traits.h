#pragma once
#include <type_traits>

namespace util
{
    template<typename T>
    using BaseType = std::decay_t<T>;  // TODO: use decay_t explicitly

    template <class T, class Arg, class... Args>
    constexpr bool IsCopyCtorArg_v = sizeof...(Args) == 0 && std::is_same<BaseType<Arg>, T>::value;
}