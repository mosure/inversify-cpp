#pragma once

#include <functional>

#include <mosure/inversify.hpp>

#include "mock/ifizz.hpp"


namespace inversify = mosure::inversify;

namespace symbols {
    using foo = inversify::Symbol<int>;
    using bar = inversify::Symbol<double>;
    using fizz = inversify::Symbol<IFizzSharedPtr>;
    using fizzFactory = inversify::Symbol<std::function<IFizzUniquePtr()>>;

    using autoFizzUnique = inversify::Symbol<IFizzUniquePtr>;
    using autoFizzShared = inversify::Symbol<IFizzSharedPtr>;
}
