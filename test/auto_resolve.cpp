#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/fizz.hpp"
#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container resolves automatic values", "[resolve]") {

    GIVEN("A container with automatic unique_ptr binding") {
        inversify::Container<
            symbols::foo,
            symbols::bar,
            symbols::autoFizzUnique,
            symbols::autoFizzShared
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::bar>().toConstantValue(1.618);

        container.bind<symbols::autoFizzUnique>().to<Fizz>();

        WHEN("the dependency is resolved") {
            auto result = container.template get<symbols::autoFizzUnique>();
            auto foo = result->buzz();

            THEN("the correct value is returned") {
                REQUIRE(foo == 10);
                REQUIRE(result->counter == 1);
            }
        }

        WHEN("multiple dependencies are resolved") {
            auto result1 = container.template get<symbols::autoFizzUnique>();
            auto result2 = container.template get<symbols::autoFizzUnique>();

            result1->buzz();
            result2->buzz();

            THEN("the values are unique") {
                REQUIRE(result1->counter == 1);
                REQUIRE(result2->counter == 1);
                REQUIRE(result1 != result2);
            }
        }
    }

    GIVEN("A container with automatic singleton shared_ptr binding") {
        inversify::Container<
            symbols::foo,
            symbols::bar,
            symbols::autoFizzUnique,
            symbols::autoFizzShared
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::bar>().toConstantValue(1.618);

        container.bind<symbols::autoFizzShared>().to<Fizz>().inSingletonScope();

        WHEN("the dependency is resolved") {
            auto result = container.template get<symbols::autoFizzShared>();
            auto foo = result->buzz();

            THEN("the correct value is returned") {
                REQUIRE(foo == 10);
                REQUIRE(result->counter == 1);
            }
        }

        WHEN("multiple dependencies are resolved") {
            auto result1 = container.template get<symbols::autoFizzShared>();
            auto result2 = container.template get<symbols::autoFizzShared>();

            result1->buzz();
            result2->buzz();

            THEN("the values are equal") {
                REQUIRE(result1->counter == 2);
                REQUIRE(result2->counter == 2);
                REQUIRE(result1 == result2);
            }
        }
    }
}
