#pragma once

#include <mosure/cache.hpp>
#include <mosure/resolver.hpp>
#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename Interface>
    struct Binding {
        using value = Interface;

        inversify::Cache cache;
        inversify::Resolver resolver;
        inversify::Symbol symbol;
    };

}
