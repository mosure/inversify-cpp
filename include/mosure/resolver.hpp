#pragma once

#include <mosure/exceptions/resolution.hpp>
#include <mosure/symbol.hpp>


namespace mosure::inversify {

    template <typename Interface>
    struct Resolver {
        virtual Interface resolve(Context& context) {
            throw inversify::exceptions::ResolutionException();
        }
    };

    // template <typename Interface>
    // struct ConstantResolver : Resolver<Interface> {
    //     Interface resolve(Context& context) override {

    //     }
    // };

    // template <typename Interface>
    // struct DynamicResolver : Resolver<Interface> {
    //     Interface resolve(Context& context) override {

    //     }
    // };

    // template <typename Interface>
    // struct CachedResolver : Resolver<Interface> {
    //     CachedResolver(Resolver<Interface> parent) { }

    //     Interface resolve(Context& context) overrie {

    //     }
    // };

}
