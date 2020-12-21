#pragma once

#include <mosure/cache.hpp>
#include <mosure/resolver.hpp>
#include <mosure/symbol.hpp>


namespace mosure::inversify {

    struct Binding {
        // inversify::Cache cache;
        // inversify::Resolver resolver;
        inversify::Symbol symbol;
    };

}
