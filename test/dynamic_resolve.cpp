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
        inversify::Container container;

        container.bind<symbols::foo>().toConstantValue(3);
        container.bind<symbols::bar>().toDynamicValue([](const inversify::Context& ctx) {
            auto foo = ctx.container.get<symbols::foo>();

            return foo * 1.5;
        });

        WHEN("the dependency is resolved") {
            auto result = container.get<symbols::bar>();

            THEN("the correct value is returned") {
                REQUIRE(result == 4.5);
            }
        }

        WHEN("the binding is redefined") {
            container.bind<symbols::bar>().toDynamicValue([](const inversify::Context& ctx) {
                auto foo = ctx.container.get<symbols::foo>();

                return foo * 2.5;
            });

            WHEN("the dependency is resolved") {
                auto result = container.get<symbols::bar>();

                THEN("the updated value is returned") {
                    REQUIRE(result == 7.5);
                }
            }
        }
    }

    GIVEN("A container with factory binding") {
        inversify::Container container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::bar>().toConstantValue(1.618);

        container.bind<symbols::fizzFactory>().toDynamicValue(
            [](const inversify::Context& ctx) {
                return [&]() {
                    auto foo = ctx.container.get<symbols::foo>();
                    auto bar = ctx.container.get<symbols::bar>();

                    auto fizz = std::make_unique<Fizz>(foo, bar);

                    return fizz;
                };
            }
        );

        WHEN("the dependency is resolved") {
            auto factory = container.get<symbols::fizzFactory>();

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
        inversify::Container container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::bar>().toConstantValue(1.618);

        container.bind<symbols::fizz>().toDynamicValue(
            [](const inversify::Context& ctx) {
                auto foo = ctx.container.get<symbols::foo>();
                auto bar = ctx.container.get<symbols::bar>();

                auto fizz = std::make_shared<Fizz>(foo, bar);

                return fizz;
            }
        ).inSingletonScope();

        WHEN("multiple dependencies are resolved") {
            auto fizz1 = container.get<symbols::fizz>();
            auto fizz2 = container.get<symbols::fizz>();

            THEN("both dependency pointers are equal") {
                REQUIRE(fizz1 == fizz2);
            }
        }
    }

    GIVEN("A container with resolution dynamic binding") {
        inversify::Container container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::bar>().toConstantValue(1.618);

        container.bind<symbols::fizz>().toDynamicValue(
            [](const inversify::Context& ctx) {
                auto foo = ctx.container.get<symbols::foo>();
                auto bar = ctx.container.get<symbols::bar>();

                auto fizz = std::make_unique<Fizz>(foo, bar);

                return fizz;
            }
        );

        WHEN("multiple dependencies are resolved") {
            auto fizz1 = container.get<symbols::fizz>();
            auto fizz2 = container.get<symbols::fizz>();

            THEN("dependencies are unique") {
                REQUIRE(fizz1 != fizz2);
            }
        }
    }
}
