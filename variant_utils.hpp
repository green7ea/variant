#pragma once

#include "variant.hpp"

#include <array>
#include <vector>

template <typename... Args>
struct static_add;

template <typename Arg>
struct static_add<Arg>
{
    static const size_t result = 1;
};

template <typename Arg, typename... Args>
struct static_add<Arg, Args...>
{
    static const size_t result =
        1 + static_add<Args...>::result;
};

template <typename... Args>
std::array< Variant<Args...>, static_add<Args...>::result >
make_var_array(const Args& ...values)
{
    return std::array< Variant<Args...>,
                       static_add<Args...>::result >
    { { Variant<Args...>(values)... } };
}

template <typename... Args>
std::vector< Variant<Args...> >
make_var_vector(const Args& ...values)
{
    return std::vector< Variant<Args...> >
    { { Variant<Args...>(values)... } };
}
