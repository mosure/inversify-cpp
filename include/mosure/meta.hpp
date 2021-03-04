#pragma once

#include <type_traits>
#include <utility>

#include <mosure/symbol.hpp>


namespace mosure::inversify::meta {

    template <class T, template <class...> class Template>
    struct is_specialization : std::false_type { };

    template <template <class...> class Template, class... Args>
    struct is_specialization<Template<Args...>, Template> : std::true_type { };

    template <typename ...Types>
    inline constexpr bool valid_inject_types_v = std::conjunction_v<
        meta::is_specialization<Types, inversify::Symbol>...
    >;

    template <typename T>
    using value_t = typename T::value;

}
