#include <functional>
#include <memory>
#include <utility>

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/fizz.hpp"
#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

struct ResolutionDependency {
    explicit ResolutionDependency(int value) : value(value) { }

    int value;
};
using ResolutionDependencyPtr = std::shared_ptr<ResolutionDependency>;

struct ResolutionConsumer {
    ResolutionConsumer(ResolutionDependencyPtr first, ResolutionDependencyPtr second)
        : first(std::move(first)), second(std::move(second))
    { }

    ResolutionDependencyPtr first;
    ResolutionDependencyPtr second;
};
using ResolutionConsumerPtr = std::shared_ptr<ResolutionConsumer>;

namespace resolution_symbols {
    using dependency = inversify::Symbol<ResolutionDependencyPtr>;
    using consumer = inversify::Symbol<ResolutionConsumerPtr>;
}

template <>
struct inversify::Injectable<ResolutionConsumer>
    : inversify::Inject<
        resolution_symbols::dependency,
        resolution_symbols::dependency
    >
{ };

SCENARIO("container resolves dynamic values", "[resolve]") {

    GIVEN("A container with dynamic binding") {
        inversify::Container<
            symbols::foo,
            symbols::bar,
            symbols::fizz,
            symbols::fizzFactory
        > container;

        container.bind<symbols::foo>().toConstantValue(3);
        container.bind<symbols::bar>().toDynamicValue([](auto& ctx) {
            auto foo = ctx.container.template get<symbols::foo>();

            return foo * 1.5;
        });

        WHEN("the dependency is resolved") {
            auto result = container.get<symbols::bar>();

            THEN("the correct value is returned") {
                REQUIRE(result == 4.5);
            }
        }

        WHEN("the binding is redefined") {
            container.bind<symbols::bar>().toDynamicValue([](auto& ctx) {
                auto foo = ctx.container.template get<symbols::foo>();

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
        inversify::Container<
            symbols::foo,
            symbols::bar,
            symbols::fizz,
            symbols::fizzFactory
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::bar>().toConstantValue(1.618);

        container.bind<symbols::fizzFactory>().toDynamicValue(
            [](auto& ctx) {
                return [&]() {
                    auto foo = ctx.container.template get<symbols::foo>();
                    auto bar = ctx.container.template get<symbols::bar>();

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
        inversify::Container<
            symbols::foo,
            symbols::bar,
            symbols::fizz,
            symbols::fizzFactory
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::bar>().toConstantValue(1.618);

        container.bind<symbols::fizz>().toDynamicValue(
            [](auto& ctx) {
                auto foo = ctx.container.template get<symbols::foo>();
                auto bar = ctx.container.template get<symbols::bar>();

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
        inversify::Container<
            symbols::foo,
            symbols::bar,
            symbols::fizz,
            symbols::fizzFactory
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::bar>().toConstantValue(1.618);

        container.bind<symbols::fizz>().toDynamicValue(
            [](auto& ctx) {
                auto foo = ctx.container.template get<symbols::foo>();
                auto bar = ctx.container.template get<symbols::bar>();

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

    GIVEN("A container with a resolution scoped dynamic binding") {
        inversify::Container<
            symbols::foo,
            resolution_symbols::dependency,
            resolution_symbols::consumer
        > container;

        container.bind<symbols::foo>().toConstantValue(10);

        int factoryCount = 0;
        container.bind<resolution_symbols::dependency>().toDynamicValue(
            [&](auto& ctx) {
                ++factoryCount;
                auto foo = ctx.container.template get<symbols::foo>();

                return std::make_shared<ResolutionDependency>(foo);
            }
        ).inResolutionScope();

        container.bind<resolution_symbols::consumer>().to<ResolutionConsumer>();

        WHEN("a graph resolves the dependency twice") {
            auto consumer = container.get<resolution_symbols::consumer>();

            THEN("the same dependency instance is reused within the graph") {
                REQUIRE(consumer->first == consumer->second);
                REQUIRE(factoryCount == 1);

                AND_THEN("a new graph receives a new instance") {
                    auto next = container.get<resolution_symbols::consumer>();

                    REQUIRE(next->first != consumer->first);
                    REQUIRE(factoryCount == 2);
                }
            }
        }
    }
}
