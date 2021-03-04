#pragma once

#include <tuple>
#include <type_traits>

#include <mosure/meta.hpp>
#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename T>
    static typename T::value get();

    template <typename... Dependencies>
    struct Inject {
        static_assert(meta::valid_inject_types_v<Dependencies...>, "inversify::Injectable dependencies must be of type inversify::Symbol");

        inline static auto resolve() {
            return std::make_tuple(inversify::get<Dependencies>()...);
        }
    };

    template <typename Implementation, typename Inject = inversify::Inject<>>
    struct Injectable : Inject { };

}
