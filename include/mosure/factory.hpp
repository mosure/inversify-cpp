#pragma once

#include <functional>

#include <mosure/context.hpp>


namespace mosure::inversify {

    template <typename T>
    using Factory = std::function<T(const inversify::Context&)>;

}
