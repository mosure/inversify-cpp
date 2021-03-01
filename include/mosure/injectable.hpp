#pragma once

#include <tuple>
#include <type_traits>

#include <mosure/context.hpp>
#include <mosure/meta.hpp>
#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename... Dependencies>
    struct Inject {
        static_assert(meta::valid_inject_types_v<Dependencies...>, "inversify::Injectable dependencies must be of type inversify::Symbol");

        inline static auto resolve(const inversify::Context& context) {
            return std::make_tuple(context.container.get<Dependencies>()...);
        }
    };

    template <typename Implementation, typename Inject = inversify::Inject<>>
    struct Injectable : Inject { };

}
