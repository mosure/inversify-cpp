#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include <mosure/symbol.hpp>


namespace mosure::inversify::meta {

template<
    typename T,
    typename... Types
>
inline constexpr bool contains_v = std::disjunction_v<
    std::is_same<T, Types>...
>;


template <typename... Args>
struct is_empty : std::false_type { };

template<>
struct is_empty<> : std::true_type { };

template <typename... Args>
inline constexpr bool is_empty_v = is_empty<Args...>::value;


template <
    typename T,
    template <typename...> typename Template
>
struct is_specialization : std::false_type { };

template <
    template <typename...> typename Template,
    typename... Args
>
struct is_specialization<Template<Args...>, Template> : std::true_type { };


template <typename ...Types>
inline constexpr bool valid_symbol_types_v = std::conjunction_v<
    meta::is_specialization<Types, inversify::Symbol>...
> || std::conjunction_v<
    std::is_base_of<inversify::Symbol<typename Types::value>, Types>...
>;

}
