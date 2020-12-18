#pragma once

#include <type_traits>
#include <vector>

#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename ...Types>
    inline constexpr bool valid_symbols_v = std::conjunction_v<
        std::is_same<inversify::symbol, Types>...
    >;

    template <class T, std::size_t = sizeof(T)>
    std::true_type is_complete_impl(T *);

    std::false_type is_complete_impl(...);

    template <class T>
    using is_complete = decltype(is_complete_impl(std::declval<T*>()));

    template <typename Dep, int size>
    struct injectable {
        inline constexpr void setTypes(inversify::symbols symbols) {
            value = symbols;
        }

        static inversify::symbols value;
    };

    // template <typename Dep, typename ...Types>
    // void injectable(Types... types) {

    //     injectable_store<Dep>::setTypes(types...);
    // }

}
