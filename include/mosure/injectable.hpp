#pragma once

#include <tuple>
#include <type_traits>

#include <mosure/context.hpp>
#include <mosure/meta.hpp>


namespace mosure::inversify {

template <typename... Dependencies>
struct Inject {
    static_assert(
        meta::valid_symbol_types_v<Dependencies...>,
        "inversify::Injectable dependencies must be of type inversify::Symbol"
    );

#ifdef INVERSIFY_BINDING_INSPECTION
    inline static std::tuple<Dependencies...> dependencies;
#endif

    template <typename... SymbolTypes>
    inline static auto resolve(const inversify::Context<SymbolTypes...>& context) {
        return std::make_tuple(context.container.template get<Dependencies>()...);
    }
};

template <typename Implementation, typename Inject = inversify::Inject<>>
struct Injectable: Inject { };

}
