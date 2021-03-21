#include <mosure/inversify.hpp>

#include "mock/iservices.hpp"
#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

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

template <>
struct inversify::Injectable<ServiceA>
    : inversify::Inject<
        symbols::foo
    >
{ };

template <>
struct inversify::Injectable<ServiceB>
    : inversify::Inject<
        symbols::symbolA
    >
{ };

template <>
struct inversify::Injectable<ServiceC>
    : inversify::Inject<
        symbols::symbolB
    >
{ };
