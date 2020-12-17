#pragma once

#include <array>
#include <type_traits>

#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename ...Types>
    inline constexpr bool valid_symbols_v = std::conjunction_v<
        std::is_same<inversify::symbol, Types>...
    >;

    template <typename Dep, typename ...Types>
    struct injectable {
        constexpr injectable(Types...) { };

        static_assert(valid_symbols_v<Types...>, "Injectable symbols must be of type mosure::inversify::symbol.");

        static constexpr std::array<
            inversify::symbol,
            sizeof...(Types)
        > value = {{ Types... }};
    };

}
