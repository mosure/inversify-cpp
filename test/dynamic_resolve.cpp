#include <functional>
#include <memory>

#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/fizz.hpp"
#include "mock/ifizz.hpp"
#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container resolves dynamic values", "[resolve]") {

    GIVEN("A container with dynamic binding") {
        inversify::bind<symbols::foo>().toConstantValue(3);
        inversify::bind<symbols::bar>().toDynamicValue([]() {
            auto foo = inversify::get<symbols::foo>();

            return foo * 1.5;
        });

        WHEN("the dependency is resolved") {
            auto result = inversify::get<symbols::bar>();

            THEN("the correct value is returned") {
                REQUIRE(result == 4.5);
            }
        }

        WHEN("the binding is redefined") {
            inversify::bind<symbols::bar>().toDynamicValue([]() {
                auto foo = inversify::get<symbols::foo>();

                return foo * 2.5;
            });

            WHEN("the dependency is resolved") {
                auto result = inversify::get<symbols::bar>();

                THEN("the updated value is returned") {
                    REQUIRE(result == 7.5);
                }
            }
        }
    }

    GIVEN("A container with factory binding") {
        inversify::bind<symbols::foo>().toConstantValue(10);
        inversify::bind<symbols::bar>().toConstantValue(1.618);

        inversify::bind<symbols::fizzFactory>().toDynamicValue(
            []() {
                return []() {
                    auto foo = inversify::get<symbols::foo>();
                    auto bar = inversify::get<symbols::bar>();

                    auto fizz = std::make_unique<Fizz>(foo, bar);

                    return fizz;
                };
            }
        );

        WHEN("the dependency is resolved") {
            auto factory = inversify::get<symbols::fizzFactory>();

            WHEN("the factory is called") {
                auto result = factory();

                THEN("A valid object is created") {
                    auto foo = result->buzz();

                    REQUIRE(foo == 10);
                    REQUIRE(result->counter == 1);
                }
            }
        }
    }

    GIVEN("A container with singleton dynamic binding") {
        inversify::bind<symbols::foo>().toConstantValue(10);
        inversify::bind<symbols::bar>().toConstantValue(1.618);

        inversify::bind<symbols::fizz>().toDynamicValue(
            []() {
                auto foo = inversify::get<symbols::foo>();
                auto bar = inversify::get<symbols::bar>();

                auto fizz = std::make_shared<Fizz>(foo, bar);

                return fizz;
            }
        ).inSingletonScope();

        WHEN("multiple dependencies are resolved") {
            auto fizz1 = inversify::get<symbols::fizz>();
            auto fizz2 = inversify::get<symbols::fizz>();

            THEN("both dependency pointers are equal") {
                REQUIRE(fizz1 == fizz2);
            }
        }
    }

    GIVEN("A container with resolution dynamic binding") {
        inversify::bind<symbols::foo>().toConstantValue(10);
        inversify::bind<symbols::bar>().toConstantValue(1.618);

        inversify::bind<symbols::fizz>().toDynamicValue(
            []() {
                auto foo = inversify::get<symbols::foo>();
                auto bar = inversify::get<symbols::bar>();

                auto fizz = std::make_unique<Fizz>(foo, bar);

                return fizz;
            }
        );

        WHEN("multiple dependencies are resolved") {
            auto fizz1 = inversify::get<symbols::fizz>();
            auto fizz2 = inversify::get<symbols::fizz>();

            THEN("dependencies are unique") {
                REQUIRE(fizz1 != fizz2);
            }
        }
    }
}
