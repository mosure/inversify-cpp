#pragma once

#include <array>
#include <type_traits>

#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <class T, class ...Types>
    inline constexpr bool valid_symbols_v = std::conjunction_v<
        std::is_same<T, Types>...
    >;

    template <class Dep, class Symbol>
    struct injectable {
        using value = Dep;
        using symbol_type = Symbol;

        template <class ...Types>
        struct dependencies {
            static_assert(valid_symbols_v<Symbol, Types...>, "Injectable symbols must be of type mosure::inversify::symbol.");

            static constexpr std::array <
                Symbol,
                sizeof...(Types)
            > value = {{ Types... }};
        };
    };

}
