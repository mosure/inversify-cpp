#include <chrono>
#include <iostream>
#include <memory>

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/symbols.hpp"
#include "mock/services.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container resolves automatic values quickly", "[performance]") {

    GIVEN("A container with an auto binding chain of size 3") {
        inversify::Container<
            symbols::foo,
            symbols::symbolA,
            symbols::symbolB,
            symbols::symbolC
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::symbolA>().to<ServiceA>();
        container.bind<symbols::symbolB>().to<ServiceB>();
        container.bind<symbols::symbolC>().to<ServiceC>();

        BENCHMARK("container.get<T>()") {
            return container.get<symbols::symbolC>();
        };

        BENCHMARK("manual injection with nested std::make_shared<T>(...)") {
            return std::make_shared<ServiceC>(
                std::make_shared<ServiceB>(
                    std::make_shared<ServiceA>(
                        10
                    )
                )
            );
        };
    }
}
