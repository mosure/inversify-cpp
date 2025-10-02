#include <chrono>
#include <iostream>
#include <memory>
#include <utility>

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/symbols.hpp"
#include "mock/services.hpp"


namespace inversify = mosure::inversify;

struct ScopedAggregator {
    ScopedAggregator(ServiceAPtr first, ServiceAPtr second)
        : first_(std::move(first)), second_(std::move(second))
    { }

    ServiceAPtr first_;
    ServiceAPtr second_;
};
using ScopedAggregatorPtr = std::shared_ptr<ScopedAggregator>;

namespace speed_symbols {
    using aggregator = inversify::Symbol<ScopedAggregatorPtr>;
}

template <>
struct inversify::Injectable<ScopedAggregator>
    : inversify::Inject<
        symbols::symbolA,
        symbols::symbolA
    >
{ };

SCENARIO("container resolves automatic values quickly", "[performance]") {

    GIVEN("A container with an auto binding chain of size 3") {
        inversify::Container<
            symbols::foo,
            symbols::symbolA,
            symbols::symbolB,
            symbols::symbolC
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::symbolA>().to<ServiceA>();
        container.bind<symbols::symbolB>().to<ServiceB>();
        container.bind<symbols::symbolC>().to<ServiceC>();

        BENCHMARK("container.get<T>()") {
            return container.get<symbols::symbolC>();
        };

        BENCHMARK("manual injection with nested std::make_shared<T>(...)") {
            return std::make_shared<ServiceC>(
                std::make_shared<ServiceB>(
                    std::make_shared<ServiceA>(
                        10
                    )
                )
            );
        };
    }

    GIVEN("A container with resolution scoped reuse within a graph") {
        inversify::Container<
            symbols::foo,
            symbols::symbolA,
            speed_symbols::aggregator
        > container;

        container.bind<symbols::foo>().toConstantValue(10);
        container.bind<symbols::symbolA>().to<ServiceA>().inResolutionScope();
        container.bind<speed_symbols::aggregator>().to<ScopedAggregator>();

        BENCHMARK("resolution scoped aggregator") {
            return container.get<speed_symbols::aggregator>();
        };
    }
}
