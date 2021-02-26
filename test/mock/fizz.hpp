#pragma once

#include <mosure/inversify.hpp>

#include "mock/ifizz.hpp"
#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

struct Fizz : IFizz {
    Fizz(int foo, double bar)
        :
        foo_(foo),
        bar_(bar)
    { }

    int buzz() override {
        counter++;

        return foo_;
    }

    int foo_;
    double bar_;
};

template <>
struct inversify::Injectable<Fizz> {
    using Inject = inversify::Inject<
        symbols::foo,
        symbols::bar
    >;
};
