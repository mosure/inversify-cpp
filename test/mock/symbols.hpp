#pragma once

#include <functional>

#include <mosure/inversify.hpp>

#include "mock/ifizz.hpp"
#include "mock/iservices.hpp"


namespace inversify = mosure::inversify;

namespace symbols {
    using foo = inversify::Symbol<int>;
    using bar = inversify::Symbol<double>;
    using fizz = inversify::Symbol<IFizzSharedPtr>;
    using fizzFactory = inversify::Symbol<std::function<IFizzUniquePtr()>>;

    using autoFizzUnique = inversify::Symbol<IFizzUniquePtr>;
    using autoFizzShared = inversify::Symbol<IFizzSharedPtr>;

    using symbolA = inversify::Symbol<ServiceAPtr>;
    using symbolB = inversify::Symbol<ServiceBPtr>;
    using symbolC = inversify::Symbol<ServiceCPtr>;
}
