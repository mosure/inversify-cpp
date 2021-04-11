#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/fizz.hpp"
#include "mock/qux.hpp"
#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container inspection yields proper metadata", "[inspection]") {

    GIVEN("A container with automatic nested bindings") {
        inversify::Container<
            symbols::foo,
            symbols::bar,
            symbols::autoFizzShared,
            symbols::autoQuxUnique
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::bar>().toConstantValue(1.618);

        container.bind<symbols::autoFizzShared>().to<Fizz>().inSingletonScope();

        auto quxResolverPtr = container.bind<symbols::autoQuxUnique>().to<Qux>().getResolver();

        WHEN("the inspection is done") {
            auto dep_tuple = quxResolverPtr->getDependencies();

            THEN("the correct values are returned") {
                REQUIRE(!dep_tuple.get<symbols::foo>());
                REQUIRE(!dep_tuple.get<symbols::bar>());
                REQUIRE(!!dep_tuple.get<symbols::autoFizzShared>());
                REQUIRE(!dep_tuple.get<symbols::autoQuxUnique>());
            }
        }
    }
}
