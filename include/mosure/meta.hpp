#pragma once

#include <type_traits>
#include <utility>

#include <mosure/symbol.hpp>


namespace mosure::inversify::meta {

    template<typename T, typename... Types>
    inline constexpr bool contains_v = std::disjunction_v<
        std::is_same<T, Types>...
    >;

    template <typename... Args>
    struct is_empty : std::false_type { };

    template<>
    struct is_empty<> : std::true_type { };

    template <typename... Args>
    inline constexpr bool is_empty_v = is_empty<Args...>::value;

    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type { };

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type { };

    template <typename T>
    using value_t = typename T::value;

    template <typename ...Types>
    inline constexpr bool valid_symbol_types_v = std::conjunction_v<
        meta::is_specialization<Types, inversify::Symbol>...
    >;

}
