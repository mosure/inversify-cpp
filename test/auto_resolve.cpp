#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/fizz.hpp"
#include "mock/ifizz.hpp"
#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container resolves automatic values", "[resolve]") {

    GIVEN("A container with automatic binding") {
        inversify::Container container {};

        container.bind<int>(symbols::foo).toConstantValue(10);
        container.bind<double>(symbols::bar).toConstantValue(1.618);

        container.bind<Fizz>(symbols::autoFizz).to<Fizz>();

        WHEN("the dependency is resolved") {
            auto result = container.get<Fizz>(symbols::autoFizz);
            auto foo = result.buzz();

            THEN("the correct value is returned") {
                REQUIRE(foo == 10);
                REQUIRE(result.counter == 1);
            }
        }

        WHEN("multiple dependencies are resolved") {
            auto result1 = container.get<Fizz>(symbols::autoFizz);
            auto result2 = container.get<Fizz>(symbols::autoFizz);

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

        container.bind<int>(symbols::foo).toConstantValue(10);
        container.bind<double>(symbols::bar).toConstantValue(1.618);

        container.bind<IFizzUniquePtr>(symbols::autoFizzUnique).to<Fizz>();

        WHEN("the dependency is resolved") {
            auto result = container.get<IFizzUniquePtr>(symbols::autoFizzUnique);
            auto foo = result->buzz();

            THEN("the correct value is returned") {
                REQUIRE(foo == 10);
                REQUIRE(result->counter == 1);
            }
        }

        WHEN("multiple dependencies are resolved") {
            auto result1 = container.get<IFizzUniquePtr>(symbols::autoFizzUnique);
            auto result2 = container.get<IFizzUniquePtr>(symbols::autoFizzUnique);

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

        container.bind<int>(symbols::foo).toConstantValue(10);
        container.bind<double>(symbols::bar).toConstantValue(1.618);

        container.bind<IFizzSharedPtr>(symbols::autoFizzShared).to<Fizz>().inSingletonScope();

        WHEN("the dependency is resolved") {
            auto result = container.get<IFizzSharedPtr>(symbols::autoFizzShared);
            auto foo = result->buzz();

            THEN("the correct value is returned") {
                REQUIRE(foo == 10);
                REQUIRE(result->counter == 1);
            }
        }

        WHEN("multiple dependencies are resolved") {
            auto result1 = container.get<IFizzSharedPtr>(symbols::autoFizzShared);
            auto result2 = container.get<IFizzSharedPtr>(symbols::autoFizzShared);

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
