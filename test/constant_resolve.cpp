#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container resolves constant values", "[resolve]") {

    GIVEN("A container with constant binding") {
        inversify::bind<symbols::foo>().toConstantValue(10);

        WHEN("the dependency is resolved") {
            auto result = inversify::get<symbols::foo>();

            THEN("the correct value is returned") {
                REQUIRE(result == 10);
            }
        }

        WHEN("the binding is redefined") {
            inversify::bind<symbols::foo>().toConstantValue(20);

            WHEN("the dependency is resolved") {
                auto result = inversify::get<symbols::foo>();

                THEN("the updated value is returned") {
                    REQUIRE(result == 20);
                }
            }
        }
    }

    // // Multiple containers not supported in static variant
    // GIVEN("Multiple containers with constant bindings") {
    //     inversify::Container container1;
    //     inversify::Container container2;

    //     container1.bind<symbols::bar>().toConstantValue(3.1415);
    //     container2.bind<symbols::bar>().toConstantValue(1.0213);

    //     WHEN("the dependency is resolved") {
    //         auto result1 = container1.get<symbols::bar>();
    //         auto result2 = container1.get<symbols::bar>();

    //         THEN("the correct value is returned") {
    //             REQUIRE(result1 == 3.1415);
    //             REQUIRE(result2 == 1.0213);
    //         }
    //     }
    // }
}
