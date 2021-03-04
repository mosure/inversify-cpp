#include <chrono>
#include <iostream>
#include <memory>

#include <catch2/catch.hpp>

#include <mosure/inversify.hpp>

#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

struct IServiceA {
    virtual int foo() = 0;
};
struct ServiceA : IServiceA {
    ServiceA(int foo)
        :
        foo_(foo)
    { }

    inline int foo() override {
        return foo_;
    }

    int foo_;
};
using ServiceAPtr = std::shared_ptr<IServiceA>;
using symbolA = inversify::Symbol<ServiceAPtr>;

struct IServiceB {
    virtual int foo() = 0;
};
struct ServiceB : IServiceB {
    ServiceB(ServiceAPtr serviceA)
        :
        serviceA_(serviceA)
    { }

    inline int foo() override {
        return serviceA_->foo();
    }

    ServiceAPtr serviceA_;
};
using ServiceBPtr = std::shared_ptr<IServiceB>;
using symbolB = inversify::Symbol<ServiceBPtr>;

struct IServiceC {
    virtual int foo() = 0;
};
struct ServiceC : IServiceC {
    ServiceC(ServiceBPtr serviceB)
        :
        serviceB_(serviceB)
    { }

    inline int foo() override {
        return serviceB_->foo();
    }

    ServiceBPtr serviceB_;
};
using ServiceCPtr = std::shared_ptr<IServiceC>;
using symbolC = inversify::Symbol<ServiceCPtr>;

template <>
struct inversify::Injectable<ServiceA>
    : inversify::Inject<
        symbols::foo
    >
{ };

template <>
struct inversify::Injectable<ServiceB>
    : inversify::Inject<
        symbolA
    >
{ };

template <>
struct inversify::Injectable<ServiceC>
    : inversify::Inject<
        symbolB
    >
{ };


SCENARIO("container resolves automatic values quickly", "[performance]") {

    GIVEN("A container with an auto binding chain") {
        inversify::bind<symbols::foo>().toConstantValue(10);
        inversify::bind<symbolA>().to<ServiceA>();
        inversify::bind<symbolB>().to<ServiceB>();
        inversify::bind<symbolC>().to<ServiceC>();

        WHEN("the dependency is resolved") {
            const int iterations = 1000000;

            auto inversify_start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < iterations; ++i) {
                inversify::get<symbolC>();
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
                REQUIRE(speed_ratio < 1.75); // these are rookie numbers
            }
        }
    }
}
