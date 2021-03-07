#include <chrono>
#include <iostream>
#include <memory>

#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/symbols.hpp"
#include "mock/services.hpp"


namespace inversify = mosure::inversify;

SCENARIO("container resolves automatic values quickly", "[performance]") {

    GIVEN("A container with an auto binding chain") {
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

        WHEN("the dependency is resolved") {
            const int iterations = 100000000;

            auto inversify_start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < iterations; ++i) {
                container.template get<symbols::symbolC>();
            }
            auto inversify_finish = std::chrono::high_resolution_clock::now();
            auto inversify_us = std::chrono::duration_cast<std::chrono::microseconds>(inversify_finish - inversify_start);
            auto inversify_mean = inversify_us / (float)iterations;
            std::cout << "inversify mean: " << inversify_mean.count() << std::endl;

            auto manual_start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < iterations; ++i) {
                std::make_shared<ServiceC>(
                    std::make_shared<ServiceB>(
                        std::make_shared<ServiceA>(
                            10
                        )
                    )
                );
            }
            auto manual_finish = std::chrono::high_resolution_clock::now();
            auto manual_us = std::chrono::duration_cast<std::chrono::microseconds>(manual_finish - manual_start);
            auto manual_mean = manual_us / (float)iterations;
            std::cout << "manual mean: " << manual_mean.count() << std::endl;

            auto speed_ratio = inversify_mean / manual_mean;
            std::cout << "ratio: " << speed_ratio << std::endl;

            THEN("the performance is comparable to a factory") {
                REQUIRE(speed_ratio < 3.0);
            }
        }
    }
}
