#pragma once

#include <mosure/inversify.hpp>

#include "mock/ifizz.hpp"
#include "mock/iqux.hpp"
#include "mock/symbols.hpp"


namespace inversify = mosure::inversify;

struct Qux : IQux {
    Qux(IFizzSharedPtr fizz)
        :
        fizz_(fizz)
    { }

    int buzz() override {
        return fizz_->buzz() + 5;
    }

    IFizzSharedPtr fizz_;
};

template <>
struct inversify::Injectable<Qux>
    : inversify::Inject<
        symbols::autoFizzShared
    >
{ };
