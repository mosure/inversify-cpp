#pragma once

#include <functional>

#include <mosure/inversify.hpp>

#include "mock/ifizz.hpp"


namespace inversify = mosure::inversify;

namespace symbols {
    inline extern inversify::Symbol<int> foo {};
    inline extern inversify::Symbol<double> bar {};
    inline extern inversify::Symbol<IFizzSharedPtr> fizz {};
    inline extern inversify::Symbol<std::function<IFizzUniquePtr()>> fizzFactory {};

    inline extern inversify::Symbol<IFizzUniquePtr> autoFizzUnique {};
    inline extern inversify::Symbol<IFizzSharedPtr> autoFizzShared {};
}
