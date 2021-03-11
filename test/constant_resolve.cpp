#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container resolves constant values", "[resolve]") {

    GIVEN("A container with constant binding") {
        inversify::Container<
            symbols::foo
        > container;

        container.bind<symbols::foo>().toConstantValue(10);

        WHEN("the dependency is resolved") {
            auto result = container.get<symbols::foo>();

            THEN("the correct value is returned") {
                REQUIRE(result == 10);
            }
        }

        WHEN("the binding is redefined") {
            container.bind<symbols::foo>().toConstantValue(20);

            WHEN("the dependency is resolved") {
                auto result = container.get<symbols::foo>();

                THEN("the updated value is returned") {
                    REQUIRE(result == 20);
                }
            }
        }
    }

    GIVEN("A container with constant bindings of the same type") {
        inversify::Container<
            symbols::foo,
            symbols::foo2
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::foo2>().toConstantValue(20);

        WHEN("the foo is resolved") {
            auto result = container.get<symbols::foo>();

            THEN("the correct value is returned") {
                REQUIRE(result == 10);
            }
        }

        WHEN("the foo2 is resolved") {
            auto result = container.get<symbols::foo2>();

            THEN("the correct value is returned") {
                REQUIRE(result == 20);
            }
        }
    }
}
