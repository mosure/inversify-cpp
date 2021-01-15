#include <functional>
#include <memory>

#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/fizz.hpp"
#include "mock/ifizz.hpp"
#include "mock/types.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container resolves dynamic values", "[resolve]") {

    GIVEN("A container with dynamic binding") {
        inversify::Container container {};

        container.bind<double>(types::foo).toConstantValue(3.1415);
        container.bind<int>(types::bar).toDynamicValue([](const inversify::Context& ctx) {
            auto foo = ctx.container.get<double>(types::foo);

            return (int)foo;
        });

        WHEN("the dependency is resolved") {
            auto result = container.get<int>(types::bar);

            THEN("the correct value is returned") {
                REQUIRE(result == 3);
            }
        }

        WHEN("the binding is redefined") {
            container.bind<int>(types::bar).toDynamicValue([](const inversify::Context& ctx) {
                auto foo = ctx.container.get<double>(types::foo);

                return (int)foo * 2;
            });

            WHEN("the dependency is resolved") {
                auto result = container.get<int>(types::bar);

                THEN("the updated value is returned") {
                    REQUIRE(result == 6);
                }
            }
        }
    }

    GIVEN("A container with factory binding") {
        inversify::Container container {};

        container.bind<int>(types::foo).toConstantValue(10);
        container.bind<double>(types::bar).toConstantValue(1.618);

        container.bind<std::function<IFizzUniquePtr()>>(types::fizzFactory).toDynamicValue(
            [](const inversify::Context& ctx) {
                return [&]() {
                    auto foo = ctx.container.get<int>(types::foo);
                    auto bar = ctx.container.get<double>(types::bar);

                    auto fizz = std::make_unique<Fizz>(foo, bar);

                    return fizz;
                };
            }
        );

        WHEN("the dependency is resolved") {
            auto factory = container.get<std::function<IFizzUniquePtr()>>(types::fizzFactory);

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
        inversify::Container container {};

        container.bind<int>(types::foo).toConstantValue(10);
        container.bind<double>(types::bar).toConstantValue(1.618);

        container.bind<IFizzSharedPtr>(types::fizz).toDynamicValue(
            [](const inversify::Context& ctx) {
                auto foo = ctx.container.get<int>(types::foo);
                auto bar = ctx.container.get<double>(types::bar);

                auto fizz = std::make_shared<Fizz>(foo, bar);

                return fizz;
            }
        ).inSingletonScope();

        WHEN("multiple dependencies are resolved") {
            auto fizz1 = container.get<IFizzSharedPtr>(types::fizz);
            auto fizz2 = container.get<IFizzSharedPtr>(types::fizz);

            THEN("both dependency pointers are equal") {
                REQUIRE(fizz1 == fizz2);
            }
        }
    }

    GIVEN("A container with resolution dynamic binding") {
        inversify::Container container {};

        container.bind<int>(types::foo).toConstantValue(10);
        container.bind<double>(types::bar).toConstantValue(1.618);

        container.bind<IFizzUniquePtr>(types::fizz).toDynamicValue(
            [](const inversify::Context& ctx) {
                auto foo = ctx.container.get<int>(types::foo);
                auto bar = ctx.container.get<double>(types::bar);

                auto fizz = std::make_unique<Fizz>(foo, bar);

                return fizz;
            }
        );

        WHEN("multiple dependencies are resolved") {
            auto fizz1 = container.get<IFizzUniquePtr>(types::fizz);
            auto fizz2 = container.get<IFizzUniquePtr>(types::fizz);

            THEN("dependencies are unique") {
                REQUIRE(fizz1 != fizz2);
            }
        }
    }
}
