#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/fizz.hpp"
#include "mock/ifizz.hpp"
#include "mock/types.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container resolves automatic values", "[resolve]") {

    GIVEN("A container with automatic binding") {
        inversify::Container container {};

        container.bind<int>(types::foo).toConstantValue(10);
        container.bind<double>(types::bar).toConstantValue(1.618);

        container.bind<Fizz>(types::autoFizz).to<Fizz>();

        WHEN("the dependency is resolved") {
            auto result = container.get<Fizz>(types::autoFizz);
            auto foo = result.buzz();

            THEN("the correct value is returned") {
                REQUIRE(foo == 10);
                REQUIRE(result.counter == 1);
            }
        }

        WHEN("multiple dependencies are resolved") {
            auto result1 = container.get<Fizz>(types::autoFizz);
            auto result2 = container.get<Fizz>(types::autoFizz);

            result1.buzz();
            result2.buzz();

            THEN("the values are unique") {
                REQUIRE(result1.counter == 1);
                REQUIRE(result2.counter == 1);
            }
        }
    }

    GIVEN("A container with automatic unique_ptr binding") {
        inversify::Container container {};

        container.bind<int>(types::foo).toConstantValue(10);
        container.bind<double>(types::bar).toConstantValue(1.618);

        container.bind<IFizzUniquePtr>(types::autoFizzUnique).to<Fizz>();

        WHEN("the dependency is resolved") {
            auto result = container.get<IFizzUniquePtr>(types::autoFizzUnique);
            auto foo = result->buzz();

            THEN("the correct value is returned") {
                REQUIRE(foo == 10);
                REQUIRE(result->counter == 1);
            }
        }

        WHEN("multiple dependencies are resolved") {
            auto result1 = container.get<IFizzUniquePtr>(types::autoFizzUnique);
            auto result2 = container.get<IFizzUniquePtr>(types::autoFizzUnique);

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
        inversify::Container container {};

        container.bind<int>(types::foo).toConstantValue(10);
        container.bind<double>(types::bar).toConstantValue(1.618);

        container.bind<IFizzSharedPtr>(types::autoFizzShared).to<Fizz>().inSingletonScope();

        WHEN("the dependency is resolved") {
            auto result = container.get<IFizzSharedPtr>(types::autoFizzShared);
            auto foo = result->buzz();

            THEN("the correct value is returned") {
                REQUIRE(foo == 10);
                REQUIRE(result->counter == 1);
            }
        }

        WHEN("multiple dependencies are resolved") {
            auto result1 = container.get<IFizzSharedPtr>(types::autoFizzShared);
            auto result2 = container.get<IFizzSharedPtr>(types::autoFizzShared);

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
